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

#include <QByteArray>

#include <map>

using namespace MessageComposer;

class MessageComposer::AutocryptHeadersJobPrivate : public ContentJobBasePrivate
{
public:
    AutocryptHeadersJobPrivate(AutocryptHeadersJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    ~AutocryptHeadersJobPrivate() override
    {
        // clean up in case of cancelled job
        for (const auto &[key, header] : gossipHeaders) {
            delete header;
        }
        gossipHeaders.clear();
    }

    void emitGpgError(const GpgME::Error &error);
    void emitNotFoundError(const QByteArray &addr, const QByteArray &fingerprint);
    void fillHeaderData(KMime::Headers::Generic *header, const QByteArray &addr, bool preferEncrypted, const QByteArray &keydata);
    void finishOnLastSubJob();

    KMime::Content *content = nullptr;
    KMime::Message *skeletonMessage = nullptr;
    // used to ensure consistent order based on key order, not random one by async subjobs delivering
    std::map<QByteArray, KMime::Headers::Generic *> gossipHeaders;

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

    for (const auto &[key, header] : gossipHeaders) {
        content->appendHeader(header);
    }
    gossipHeaders.clear();
    resultContent = content;

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
        QString::fromLocal8Bit(error.asString()));
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

void AutocryptHeadersJobPrivate::fillHeaderData(KMime::Headers::Generic *header, const QByteArray &addr, bool preferEncrypted, const QByteArray &keydata)
{
    QByteArray parameters = "addr=" + addr + "; ";
    if (preferEncrypted) {
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

void AutocryptHeadersJob::setContent(KMime::Content *content)
{
    Q_D(AutocryptHeadersJob);

    d->content = content;
    if (content) {
        d->content->assemble();
    }
}

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
    if (!d->content) {
        Q_ASSERT(d->subjobContents.size() == 1);
        d->content = d->subjobContents.constFirst();
    }

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

            auto autocrypt = new KMime::Headers::Generic("Autocrypt");
            d->fillHeaderData(autocrypt, d->skeletonMessage->from()->addresses()[0], d->preferEncrypted, keydata);

            d->skeletonMessage->setHeader(autocrypt);
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

            auto header = new KMime::Headers::Generic("Autocrypt-Gossip");
            d->fillHeaderData(header, key.userID(0).email(), false, keydata);

            d->gossipHeaders.insert({QByteArray(key.primaryFingerprint()), header});

            d->finishOnLastSubJob();
        });

        d->subJobs++;
        gossipJob->start(QStringList(QString::fromLatin1(key.primaryFingerprint())));
        gossipJob->setExportFlags(GpgME::Context::ExportMinimal);
    }
    if (d->subJobs == 0) {
        d->resultContent = d->content;
        emitResult();
    }
}

#include "moc_autocryptheadersjob.cpp"
