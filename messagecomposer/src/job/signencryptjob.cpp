/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/signencryptjob.h"

#include "contentjobbase_p.h"
#include "job/protectedheadersjob.h"
#include "utils/util_p.h"

#include <QGpgME/Protocol>
#include <QGpgME/SignEncryptJob>

#include "messagecomposer_debug.h"
#include <kmime/kmime_message.h>
#include <kmime/kmime_content.h>
#include <kmime/kmime_headers.h>

#include <gpgme++/global.h>
#include <gpgme++/signingresult.h>
#include <gpgme++/encryptionresult.h>
#include <sstream>

using namespace MessageComposer;

class MessageComposer::SignEncryptJobPrivate : public ContentJobBasePrivate
{
public:
    SignEncryptJobPrivate(SignEncryptJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    std::vector<GpgME::Key> signers;

    std::vector<GpgME::Key> encKeys;
    QStringList recipients;
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

    Q_DECLARE_PUBLIC(SignEncryptJob)
};

SignEncryptJob::SignEncryptJob(QObject *parent)
    : ContentJobBase(*new SignEncryptJobPrivate(this), parent)
{
}

SignEncryptJob::~SignEncryptJob()
{
}

void SignEncryptJob::setContent(KMime::Content *content)
{
    Q_D(SignEncryptJob);

    Q_ASSERT(content);

    d->content = content;
}

void SignEncryptJob::setCryptoMessageFormat(Kleo::CryptoMessageFormat format)
{
    Q_D(SignEncryptJob);

    // There *must* be a concrete format set at this point.
    Q_ASSERT(format == Kleo::OpenPGPMIMEFormat
             || format == Kleo::InlineOpenPGPFormat
             || format == Kleo::SMIMEFormat
             || format == Kleo::SMIMEOpaqueFormat);
    d->format = format;
}

void SignEncryptJob::setSigningKeys(const std::vector<GpgME::Key> &signers)
{
    Q_D(SignEncryptJob);

    d->signers = signers;
}

KMime::Content *SignEncryptJob::origContent()
{
    Q_D(SignEncryptJob);

    return d->content;
}

void SignEncryptJob::setEncryptionKeys(const std::vector<GpgME::Key> &keys)
{
    Q_D(SignEncryptJob);

    d->encKeys = keys;
}

void SignEncryptJob::setRecipients(const QStringList &recipients)
{
    Q_D(SignEncryptJob);

    d->recipients = recipients;
}

void SignEncryptJob::setSkeletonMessage(KMime::Message *skeletonMessage)
{
    Q_D(SignEncryptJob);

    d->skeletonMessage = skeletonMessage;
}

void SignEncryptJob::setProtectedHeaders(bool protectedHeaders)
{
    Q_D(SignEncryptJob);

    d->protectedHeaders = protectedHeaders;
}

void SignEncryptJob::setProtectedHeadersObvoscate(bool protectedHeadersObvoscate)
{
    Q_D(SignEncryptJob);

    d->protectedHeadersObvoscate = protectedHeadersObvoscate;
}

QStringList SignEncryptJob::recipients() const
{
    Q_D(const SignEncryptJob);

    return d->recipients;
}

std::vector<GpgME::Key> SignEncryptJob::encryptionKeys() const
{
    Q_D(const SignEncryptJob);

    return d->encKeys;
}

void SignEncryptJob::doStart()
{
    Q_D(SignEncryptJob);
    Q_ASSERT(d->resultContent == nullptr);   // Not processed before.

    if (d->protectedHeaders && d->skeletonMessage && d->format & Kleo::OpenPGPMIMEFormat) {
        auto *pJob = new ProtectedHeadersJob;
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

void SignEncryptJob::slotResult(KJob *job)
{
    //Q_D(SignEncryptJob);
    if (error()) {
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

void SignEncryptJob::process()
{
    Q_D(SignEncryptJob);
    Q_ASSERT(d->resultContent == nullptr);   // Not processed before.

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
        return;
    }
    Q_ASSERT(proto);
    //d->resultContent = new KMime::Content;

    qCDebug(MESSAGECOMPOSER_LOG) << "creating signencrypt from:" << proto->name() << proto->displayName();
    std::unique_ptr<QGpgME::SignEncryptJob> job(proto->signEncryptJob(!d->binaryHint(d->format), d->format == Kleo::InlineOpenPGPFormat));
    QByteArray encBody;
    d->content->assemble();

    // replace simple LFs by CRLFs for all MIME supporting CryptPlugs
    // according to RfC 2633, 3.1.1 Canonicalization
    QByteArray content;
    if (d->format & Kleo::InlineOpenPGPFormat) {
        content = d->content->body();
    } else if (!(d->format & Kleo::SMIMEOpaqueFormat)) {
        content = KMime::LFtoCRLF(d->content->encodedContent());
    } else {                    // SMimeOpaque doesn't need LFtoCRLF, else it gets munged
        content = d->content->encodedContent();
    }

    // FIXME: Make this async
    const std::pair<GpgME::SigningResult, GpgME::EncryptionResult> res = job->exec(d->signers, d->encKeys,
                                                                                   content,
                                                                                   false,
                                                                                   encBody);

    // exec'ed jobs don't delete themselves
    job->deleteLater();

    if (res.first.error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "signing failed:" << res.first.error().asString();
        setError(res.first.error().code());
        setErrorText(QString::fromLocal8Bit(res.first.error().asString()));
        emitResult();
        return;
    }
    if (res.second.error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "encrypting failed:" << res.second.error().asString();
        setError(res.second.error().code());
        setErrorText(QString::fromLocal8Bit(res.second.error().asString()));
        emitResult();
        return;
    }

    const QByteArray signatureHashAlgo = res.first.createdSignature(0).hashAlgorithmAsString();

    d->resultContent = MessageComposer::Util::composeHeadersAndBody(d->content, encBody, d->format, false, signatureHashAlgo);

    emitResult();
}
