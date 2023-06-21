/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/encryptjob.h"

#include "contentjobbase_p.h"
#include "job/protectedheadersjob.h"
#include "utils/util_p.h"

#include <QGpgME/EncryptJob>
#include <QGpgME/Protocol>

#include "messagecomposer_debug.h"

#include <gpgme++/encryptionresult.h>
#include <gpgme++/global.h>
#include <gpgme++/signingresult.h>
#include <sstream>

using namespace MessageComposer;

class MessageComposer::EncryptJobPrivate : public ContentJobBasePrivate
{
public:
    EncryptJobPrivate(EncryptJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    QString gnupgHome;
    QStringList recipients;
    std::vector<GpgME::Key> keys;
    Kleo::CryptoMessageFormat format;
    KMime::Content *content = nullptr;
    KMime::Message *skeletonMessage = nullptr;

    bool protectedHeaders = true;
    bool protectedHeadersObvoscate = false;

    // copied from messagecomposer.cpp
    bool binaryHint(Kleo::CryptoMessageFormat f)
    {
        switch (f) {
        case Kleo::SMIMEFormat:
        case Kleo::SMIMEOpaqueFormat:
            return true;
        default:
        case Kleo::OpenPGPMIMEFormat:
        case Kleo::InlineOpenPGPFormat:
            return false;
        }
    }

    GpgME::SignatureMode signingMode(Kleo::CryptoMessageFormat f)
    {
        switch (f) {
        case Kleo::SMIMEOpaqueFormat:
            return GpgME::NormalSignatureMode;
        case Kleo::InlineOpenPGPFormat:
            return GpgME::Clearsigned;
        default:
        case Kleo::SMIMEFormat:
        case Kleo::OpenPGPMIMEFormat:
            return GpgME::Detached;
        }
    }

    Q_DECLARE_PUBLIC(EncryptJob)
};

EncryptJob::EncryptJob(QObject *parent)
    : ContentJobBase(*new EncryptJobPrivate(this), parent)
{
}

EncryptJob::~EncryptJob() = default;

void EncryptJob::setContent(KMime::Content *content)
{
    Q_D(EncryptJob);

    d->content = content;
    d->content->assemble();
}

void EncryptJob::setCryptoMessageFormat(Kleo::CryptoMessageFormat format)
{
    Q_D(EncryptJob);

    d->format = format;
}

void EncryptJob::setEncryptionKeys(const std::vector<GpgME::Key> &keys)
{
    Q_D(EncryptJob);

    d->keys = keys;
}

void EncryptJob::setRecipients(const QStringList &recipients)
{
    Q_D(EncryptJob);

    d->recipients = recipients;
}

void EncryptJob::setSkeletonMessage(KMime::Message *skeletonMessage)
{
    Q_D(EncryptJob);

    d->skeletonMessage = skeletonMessage;
}

void EncryptJob::setProtectedHeaders(bool protectedHeaders)
{
    Q_D(EncryptJob);

    d->protectedHeaders = protectedHeaders;
}

void EncryptJob::setProtectedHeadersObvoscate(bool protectedHeadersObvoscate)
{
    Q_D(EncryptJob);

    d->protectedHeadersObvoscate = protectedHeadersObvoscate;
}

void EncryptJob::setGnupgHome(const QString &path)
{
    Q_D(EncryptJob);

    d->gnupgHome = path;
}

QStringList EncryptJob::recipients() const
{
    Q_D(const EncryptJob);

    return d->recipients;
}

std::vector<GpgME::Key> EncryptJob::encryptionKeys() const
{
    Q_D(const EncryptJob);

    return d->keys;
}

void EncryptJob::doStart()
{
    Q_D(EncryptJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.

    if (d->keys.size() == 0) { // should not happen---resolver should have dealt with it earlier
        qCDebug(MESSAGECOMPOSER_LOG) << "HELP! Encrypt job but have no keys to encrypt with.";
        return;
    }

    // if setContent hasn't been called, we assume that a subjob was added
    // and we want to use that
    if (!d->content || !d->content->hasContent()) {
        if (d->subjobContents.size() == 1) {
            d->content = d->subjobContents.constFirst();
        }
    }

    if (d->protectedHeaders && d->skeletonMessage && d->format & Kleo::OpenPGPMIMEFormat) {
        auto pJob = new ProtectedHeadersJob;
        pJob->setContent(d->content);
        pJob->setSkeletonMessage(d->skeletonMessage);
        pJob->setObvoscate(d->protectedHeadersObvoscate);
        QObject::connect(pJob, &ProtectedHeadersJob::finished, this, [d, pJob](KJob *job) {
            if (job->error()) {
                return;
            }
            d->content = pJob->content();
        });
        appendSubjob(pJob);
    }

    ContentJobBase::doStart();
}

void EncryptJob::slotResult(KJob *job)
{
    // Q_D(EncryptJob);
    if (error() || job->error()) {
        ContentJobBase::slotResult(job);
        return;
    }
    if (subjobs().size() == 2) {
        auto pjob = static_cast<ProtectedHeadersJob *>(subjobs().last());
        if (pjob) {
            auto cjob = qobject_cast<ContentJobBase *>(job);
            Q_ASSERT(cjob);
            pjob->setContent(cjob->content());
        }
    }

    ContentJobBase::slotResult(job);
}

void EncryptJob::process()
{
    Q_D(EncryptJob);

    // if setContent hasn't been called, we assume that a subjob was added
    // and we want to use that
    if (!d->content || !d->content->hasContent()) {
        Q_ASSERT(d->subjobContents.size() == 1);
        d->content = d->subjobContents.constFirst();
    }

    const QGpgME::Protocol *proto = nullptr;
    if (d->format & Kleo::AnyOpenPGP) {
        proto = QGpgME::openpgp();
    } else if (d->format & Kleo::AnySMIME) {
        proto = QGpgME::smime();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "HELP! Encrypt job but have protocol to encrypt with.";
        return;
    }

    Q_ASSERT(proto);

    // for now just do the main recipients
    QByteArray content;
    d->content->assemble();
    if (d->format & Kleo::InlineOpenPGPFormat) {
        content = d->content->body();
    } else {
        content = d->content->encodedContent();
    }

    qCDebug(MESSAGECOMPOSER_LOG) << "got backend, starting job";
    QGpgME::EncryptJob *eJob = proto->encryptJob(!d->binaryHint(d->format), d->format == Kleo::InlineOpenPGPFormat);

    if (!d->gnupgHome.isEmpty()) {
        QGpgME::Job::context(eJob)->setEngineHomeDirectory(d->gnupgHome.toUtf8().constData());
    }

    QObject::connect(
        eJob,
        &QGpgME::EncryptJob::result,
        this,
        [this, d](const GpgME::EncryptionResult &result, const QByteArray &cipherText, const QString &auditLogAsHtml, const GpgME::Error &auditLogError) {
            Q_UNUSED(auditLogAsHtml)
            Q_UNUSED(auditLogError)
            if (result.error()) {
                setError(result.error().code());
                setErrorText(QString::fromLocal8Bit(result.error().asString()));
                emitResult();
                return;
            }
            d->resultContent = MessageComposer::Util::composeHeadersAndBody(d->content, cipherText, d->format, false);

            emitResult();
        });

    const auto error = eJob->start(d->keys, content, true);
    if (error.code()) {
        eJob->deleteLater();
        setError(error.code());
        setErrorText(QString::fromLocal8Bit(error.asString()));
        emitResult();
    }
}

#include "moc_encryptjob.cpp"
