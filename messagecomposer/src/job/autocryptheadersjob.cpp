/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/autocryptheadersjob.h"

#include "contentjobbase_p.h"

#include "job/singlepartjob.h"
#include "utils/util_p.h"

#include <QGpgME/ExportJob>
#include <QGpgME/Protocol>
#include <gpgme++/context.h>

#include <KCodecs>
#include <KLocalizedString>
#include <KMime/Content>
#include <KMime/Headers>
#include <Libkleo/Formatting>

#include <QByteArray>

#include <map>

using namespace MessageComposer;

class MessageComposer::AutocryptHeadersJobPrivate : public ContentJobBasePrivate
{
public:
    explicit AutocryptHeadersJobPrivate(AutocryptHeadersJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    ~AutocryptHeadersJobPrivate() override = default;

    void emitGpgError(const GpgME::Error &error);
    void emitNotFoundError(const QByteArray &addr, const QByteArray &fingerprint);
    void fillHeaderData(KMime::Headers::Generic *header, const QByteArray &addr, bool doPreferEncrypted, const QByteArray &keydata);
    void finishOnLastSubJob();

    std::unique_ptr<KMime::Content> content;
    KMime::Message *skeletonMessage = nullptr;
    // used to ensure consistent order based on key order, not random one by async subjobs delivering
    std::map<QByteArray, std::unique_ptr<KMime::Headers::Generic>> gossipHeaders;

    bool preferEncrypted = false;
    int subJobs = 0;

    QString gnupgHome;
    GpgME::Key recipientKey;
    std::vector<GpgME::Key> gossipKeys;

    Q_DECLARE_PUBLIC(AutocryptHeadersJob)
};

void AutocryptHeadersJobPrivate::finishOnLastSubJob()
{
    Q_Q(AutocryptHeadersJob);

    if (subJobs > 0) {
        return;
    }

    for (auto &[key, header] : gossipHeaders) {
        content->appendHeader(std::move(header));
    }
    gossipHeaders.clear();
    resultContent = std::move(content);

    q->emitResult();
}

void AutocryptHeadersJobPrivate::emitGpgError(const GpgME::Error &error)
{
    Q_Q(AutocryptHeadersJob);

    Q_ASSERT(error);
    const QString msg = i18n(
        "<p>An error occurred while trying to export "
        "the key from the backend:</p>"
        "<p><b>%1</b></p>",
        Kleo::Formatting::errorAsString(error));
    q->setError(KJob::UserDefinedError);
    q->setErrorText(msg);
    q->emitResult();
}

void AutocryptHeadersJobPrivate::emitNotFoundError(const QByteArray &addr, const QByteArray &fingerprint)
{
    Q_Q(AutocryptHeadersJob);
    const QString msg = i18n(
        "<p>An error occurred while trying to export "
        "the key from the backend:</p>"
        "<p><b>No valid key found for user %1 (%2)</b></p>",
        QString::fromLatin1(addr),
        QString::fromLatin1(fingerprint));
    q->setError(KJob::UserDefinedError);
    q->setErrorText(msg);
    q->emitResult();
}

void AutocryptHeadersJobPrivate::fillHeaderData(KMime::Headers::Generic *header, const QByteArray &addr, bool doPreferEncrypted, const QByteArray &keydata)
{
    QByteArray parameters = "addr=" + addr + "; ";
    if (doPreferEncrypted) {
        parameters += "prefer-encrypt=mutual; ";
    }
    parameters += "keydata=\n ";
    const auto encoded = KCodecs::base64Encode(keydata).replace('\n', QByteArray());
    const auto length = encoded.size();
    const auto lineLength = 76;
    auto start = 0;
    auto column = 1;
    while (start < length) {
        const auto midLength = std::min<int>(length - start, lineLength - column);
        parameters += encoded.mid(start, midLength);
        start += midLength;
        column += midLength;
        if (column >= lineLength) {
            parameters += "\n ";
            column = 1;
        }
    }
    header->from7BitString(parameters);
}

AutocryptHeadersJob::AutocryptHeadersJob(QObject *parent)
    : ContentJobBase(*new AutocryptHeadersJobPrivate(this), parent)
{
}

AutocryptHeadersJob::~AutocryptHeadersJob() = default;

void AutocryptHeadersJob::setSkeletonMessage(KMime::Message *skeletonMessage)
{
    Q_D(AutocryptHeadersJob);

    d->skeletonMessage = skeletonMessage;
}

void AutocryptHeadersJob::setGnupgHome(const QString &path)
{
    Q_D(AutocryptHeadersJob);

    d->gnupgHome = path;
}

void AutocryptHeadersJob::setSenderKey(const GpgME::Key &key)
{
    Q_D(AutocryptHeadersJob);

    d->recipientKey = key;
}

void AutocryptHeadersJob::setPreferEncrypted(bool preferEncrypted)
{
    Q_D(AutocryptHeadersJob);

    d->preferEncrypted = preferEncrypted;
}

void AutocryptHeadersJob::setGossipKeys(const std::vector<GpgME::Key> &gossipKeys)
{
    Q_D(AutocryptHeadersJob);

    d->gossipKeys = gossipKeys;
}

void AutocryptHeadersJob::process()
{
    Q_D(AutocryptHeadersJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.

    // if setContent hasn't been called, we assume that a subjob was added
    // and we want to use that
    Q_ASSERT(!d->content);
    Q_ASSERT(d->subjobContents.size() == 1);
    d->content = std::move(d->subjobContents.front());

    auto job = QGpgME::openpgp()->publicKeyExportJob(false);
    Q_ASSERT(job);

    if (!d->gnupgHome.isEmpty()) {
        QGpgME::Job::context(job)->setEngineHomeDirectory(d->gnupgHome.toUtf8().constData());
    }
    if (!d->recipientKey.isNull() && !d->recipientKey.isInvalid()) {
        connect(job, &QGpgME::ExportJob::result, this, [this, d](const GpgME::Error &error, const QByteArray &keydata) {
            d->subJobs--;
            if (AutocryptHeadersJob::error()) {
                // When the job already has failed do nothing.
                return;
            }
            if (error) {
                d->emitGpgError(error);
                return;
            }
            if (keydata.isEmpty()) {
                d->emitNotFoundError(d->skeletonMessage->from()->addresses()[0], d->recipientKey.primaryFingerprint());
                return;
            }

            auto autocrypt = std::make_unique<KMime::Headers::Generic>("Autocrypt");
            d->fillHeaderData(autocrypt.get(), d->skeletonMessage->from()->addresses()[0], d->preferEncrypted, keydata);

            d->skeletonMessage->setHeader(std::move(autocrypt));
            d->skeletonMessage->assemble();

            d->finishOnLastSubJob();
        });
        d->subJobs++;
        job->start(QStringList(QString::fromLatin1(d->recipientKey.primaryFingerprint())));
        job->setExportFlags(GpgME::Context::ExportMinimal);
    }

    const auto keys = d->gossipKeys;
    for (const auto &key : keys) {
        if (QByteArray(key.primaryFingerprint()) == QByteArray(d->recipientKey.primaryFingerprint())) {
            continue;
        }

        auto gossipJob = QGpgME::openpgp()->publicKeyExportJob(false);
        Q_ASSERT(gossipJob);

        if (!d->gnupgHome.isEmpty()) {
            QGpgME::Job::context(gossipJob)->setEngineHomeDirectory(d->gnupgHome.toUtf8().constData());
        }

        connect(gossipJob, &QGpgME::ExportJob::result, this, [this, d, key](const GpgME::Error &error, const QByteArray &keydata) {
            d->subJobs--;
            if (AutocryptHeadersJob::error()) {
                // When the job already has failed do nothing.
                return;
            }
            if (error) {
                d->emitGpgError(error);
                return;
            }
            if (keydata.isEmpty()) {
                d->emitNotFoundError(key.userID(0).email(), key.primaryFingerprint());
                return;
            }

            auto header = std::make_unique<KMime::Headers::Generic>("Autocrypt-Gossip");
            d->fillHeaderData(header.get(), key.userID(0).email(), false, keydata);

            d->gossipHeaders.insert({QByteArray(key.primaryFingerprint()), std::move(header)});

            d->finishOnLastSubJob();
        });

        d->subJobs++;
        gossipJob->start(QStringList(QString::fromLatin1(key.primaryFingerprint())));
        gossipJob->setExportFlags(GpgME::Context::ExportMinimal);
    }
    if (d->subJobs == 0) {
        d->resultContent = std::move(d->content);
        emitResult();
    }
}

#include "moc_autocryptheadersjob.cpp"
