/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/autocryptheadersjob.h"

#include "job/singlepartjob.h"
#include "utils/util_p.h"

#include "messagecomposer_debug.h"

#include <QTimer>

#include <QGpgME/Protocol>
#include <QGpgME/ExportJob>

#include <KCodecs/KCodecs>
#include <KMime/Headers>
#include <KMime/Content>
#include <KLocalizedString>


using namespace MessageComposer;

class MessageComposer::AutocryptHeadersJobPrivate
{
public:
    AutocryptHeadersJobPrivate(AutocryptHeadersJob *qq)
        : q_ptr(qq)
    {
    }

    void emitGpgError(const GpgME::Error &error);
    void emitNotFoundError(const QByteArray &addr, const QByteArray &fingerprint);
    void fillHeaderData(KMime::Headers::Generic* header, const QByteArray& addr, bool preferEncrypted, const QByteArray& keydata);

    AutocryptHeadersJob *q_ptr;

    KMime::Content *content = nullptr;
    KMime::Message *skeletonMessage = nullptr;

    bool preferEncrypted = false;
    int subJobs = 0;
    GpgME::Key recipientKey;
    std::vector<GpgME::Key> gossipKeys;

    Q_DECLARE_PUBLIC(AutocryptHeadersJob)
};

void AutocryptHeadersJobPrivate::emitGpgError(const GpgME::Error &error)
{
    Q_Q(AutocryptHeadersJob);

    Q_ASSERT(error);
    const QString msg = i18n("<p>An error occurred while trying to export "
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
    const QString msg = i18n("<p>An error occurred while trying to export "
                             "the key from the backend:</p>"
                             "<p><b>No vaild key found for user %1 (%2)</b></p>",
                             QString::fromLatin1(addr),
                             QString::fromLatin1(fingerprint));
    q->setError(KJob::UserDefinedError);
    q->setErrorText(msg);
    q->emitResult();
    return;
}

void AutocryptHeadersJobPrivate::fillHeaderData(KMime::Headers::Generic* header,
                                                const QByteArray& addr, bool preferEncrypted, const QByteArray& keydata)
{
    QByteArray parameters = "addr=" + addr + "; ";
    if (preferEncrypted) {
        parameters += "prefer-encrypt=mutual; ";
    }
    parameters += "keydata=\n ";
    auto encoded = KCodecs::base64Encode(keydata).replace('\n', QByteArray());
    const auto length = encoded.size();
    const auto lineLength = 76;
    auto start = 0;
    auto column = 1;
    while(start < length) {
        const auto midLength = std::min(length-start, lineLength-column);
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
    : KJob(parent)
    , d_ptr(new AutocryptHeadersJobPrivate(this))
{
}

AutocryptHeadersJob::~AutocryptHeadersJob()
{
}

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

void AutocryptHeadersJob::setRecipientKey(const GpgME::Key& key)
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

void AutocryptHeadersJob::start()
{
    QTimer::singleShot(0, this, &AutocryptHeadersJob::doStart);
}

void AutocryptHeadersJob::doStart()
{
    Q_D(AutocryptHeadersJob);

    auto job = QGpgME::openpgp()->publicKeyExportJob(false);
    Q_ASSERT(job);

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
        }

        auto autocrypt = new KMime::Headers::Generic("Autocrypt");
        d->fillHeaderData(autocrypt, d->skeletonMessage->from()->addresses()[0], d->preferEncrypted, keydata);

        d->skeletonMessage->setHeader(autocrypt);

        if (!d->subJobs) {
            emitResult();
        }
    });

    foreach(const auto key, d->gossipKeys) {
        auto gossipJob = QGpgME::openpgp()->publicKeyExportJob(false);
        Q_ASSERT(gossipJob);

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
            }

            auto header = new KMime::Headers::Generic("Autocrypt-Gossip");
            d->fillHeaderData(header, key.userID(0).email(), false, keydata);

            d->content->appendHeader(header);

            if (!d->subJobs) {
                emitResult();
            }
        });

        d->subJobs++;
        gossipJob->start(QStringList(QString::fromLatin1(key.primaryFingerprint())));

    }

    d->subJobs++;
    job->start(QStringList(QString::fromLatin1(d->recipientKey.primaryFingerprint())));
}
