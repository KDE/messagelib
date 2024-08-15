/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/signjob.h"

#include "contentjobbase_p.h"
#include "job/protectedheadersjob.h"
#include "utils/util_p.h"

#include <QGpgME/Protocol>
#include <QGpgME/SignJob>
#include <QList>

#include "messagecomposer_debug.h"
#include <KMime/Content>
#include <KMime/Headers>
#include <KMime/Message>

#include <gpgme++/encryptionresult.h>
#include <gpgme++/global.h>
#include <gpgme++/signingresult.h>
#include <sstream>

using namespace MessageComposer;

class MessageComposer::SignJobPrivate : public ContentJobBasePrivate
{
public:
    SignJobPrivate(SignJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    KMime::Content *content = nullptr;
    KMime::Message *skeletonMessage = nullptr;
    std::vector<GpgME::Key> signers;
    Kleo::CryptoMessageFormat format;

    bool protectedHeaders = true;

    // copied from messagecomposer.cpp
    [[nodiscard]] bool binaryHint(Kleo::CryptoMessageFormat f)
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

    [[nodiscard]] GpgME::SignatureMode signingMode(Kleo::CryptoMessageFormat f)
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

    Q_DECLARE_PUBLIC(SignJob)
};

SignJob::SignJob(QObject *parent)
    : ContentJobBase(*new SignJobPrivate(this), parent)
{
}

SignJob::~SignJob() = default;

void SignJob::setContent(KMime::Content *content)
{
    Q_D(SignJob);

    d->content = content;
}

void SignJob::setCryptoMessageFormat(Kleo::CryptoMessageFormat format)
{
    Q_D(SignJob);

    // There *must* be a concrete format set at this point.
    Q_ASSERT(format == Kleo::OpenPGPMIMEFormat || format == Kleo::InlineOpenPGPFormat || format == Kleo::SMIMEFormat || format == Kleo::SMIMEOpaqueFormat);
    d->format = format;
}

void SignJob::setSigningKeys(const std::vector<GpgME::Key> &signers)
{
    Q_D(SignJob);

    d->signers = signers;
}

void SignJob::setSkeletonMessage(KMime::Message *skeletonMessage)
{
    Q_D(SignJob);

    d->skeletonMessage = skeletonMessage;
}

void SignJob::setProtectedHeaders(bool protectedHeaders)
{
    Q_D(SignJob);

    d->protectedHeaders = protectedHeaders;
}

KMime::Content *SignJob::origContent()
{
    Q_D(SignJob);

    return d->content;
}

void SignJob::doStart()
{
    Q_D(SignJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.

    if (d->protectedHeaders && d->skeletonMessage && d->format & Kleo::OpenPGPMIMEFormat) {
        auto pJob = new ProtectedHeadersJob;
        pJob->setContent(d->content);
        pJob->setSkeletonMessage(d->skeletonMessage);
        pJob->setObvoscate(false);
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

void SignJob::slotResult(KJob *job)
{
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

void SignJob::process()
{
    Q_D(SignJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.

    // if setContent hasn't been called, we assume that a subjob was added
    // and we want to use that
    if (!d->content) {
        Q_ASSERT(d->subjobContents.size() == 1);
        d->content = d->subjobContents.constFirst();
    }

    // d->resultContent = new KMime::Content;

    const QGpgME::Protocol *proto = nullptr;
    if (d->format & Kleo::AnyOpenPGP) {
        proto = QGpgME::openpgp();
    } else if (d->format & Kleo::AnySMIME) {
        proto = QGpgME::smime();
    }

    Q_ASSERT(proto);

    qCDebug(MESSAGECOMPOSER_LOG) << "creating signJob from:" << proto->name() << proto->displayName();
    // for now just do the main recipients
    QByteArray signature;

    d->content->assemble();

    // replace simple LFs by CRLFs for all MIME supporting CryptPlugs
    // according to RfC 2633, 3.1.1 Canonicalization
    QByteArray content;
    if (d->format & Kleo::InlineOpenPGPFormat) {
        content = d->content->body();
    } else if (!(d->format & Kleo::SMIMEOpaqueFormat)) {
        // replace "From " and "--" at the beginning of lines
        // with encoded versions according to RfC 3156, 3
        // Note: If any line begins with the string "From ", it is strongly
        //   suggested that either the Quoted-Printable or Base64 MIME encoding
        //   be applied.
        const auto cte = d->content->contentTransferEncoding(false);
        const auto encoding = cte ? cte->encoding() : KMime::Headers::CE7Bit;
        if ((encoding == KMime::Headers::CEquPr || encoding == KMime::Headers::CE7Bit) && !d->content->contentType(false)) {
            QByteArray body = d->content->encodedBody();
            bool changed = false;
            QList<QByteArray> search;
            search.reserve(3);
            QList<QByteArray> replacements;
            replacements.reserve(3);
            search << "From "
                   << "from "
                   << "-";
            replacements << "From=20"
                         << "from=20"
                         << "=2D";

            if (encoding == KMime::Headers::CE7Bit) {
                for (int i = 0, total = search.size(); i < total; ++i) {
                    const auto pos = body.indexOf(search[i]);
                    if (pos == 0 || (pos > 0 && body.at(pos - 1) == '\n')) {
                        changed = true;
                        break;
                    }
                }
                if (changed) {
                    d->content->contentTransferEncoding()->setEncoding(KMime::Headers::CEquPr);
                    d->content->assemble();
                    body = d->content->encodedBody();
                }
            }

            for (int i = 0; i < search.size(); ++i) {
                const auto pos = body.indexOf(search[i]);
                if (pos == 0 || (pos > 0 && body.at(pos - 1) == '\n')) {
                    changed = true;
                    body.replace(pos, search[i].size(), replacements[i]);
                }
            }

            if (changed) {
                qCDebug(MESSAGECOMPOSER_LOG) << "Content changed";
                d->content->setEncodedBody(body);
            }
        }

        content = KMime::LFtoCRLF(d->content->encodedContent());
    } else { // SMimeOpaque doesn't need LFtoCRLF, else it gets munged
        content = d->content->encodedContent();
    }

    QGpgME::SignJob *job(proto->signJob(!d->binaryHint(d->format), d->format == Kleo::InlineOpenPGPFormat));
    QObject::connect(
        job,
        &QGpgME::SignJob::result,
        this,
        [this, d](const GpgME::SigningResult &result, const QByteArray &signature, const QString &auditLogAsHtml, const GpgME::Error &auditLogError) {
            Q_UNUSED(auditLogAsHtml)
            Q_UNUSED(auditLogError)
            if (result.error().code()) {
                qCDebug(MESSAGECOMPOSER_LOG) << "signing failed:" << result.error().asString();
                //        job->showErrorDialog( globalPart()->parentWidgetForGui() );
                setError(result.error().code());
                setErrorText(QString::fromLocal8Bit(result.error().asString()));
                emitResult();
                return;
            }

            QByteArray signatureHashAlgo = result.createdSignature(0).hashAlgorithmAsString();
            d->resultContent = MessageComposer::Util::composeHeadersAndBody(d->content, signature, d->format, true, signatureHashAlgo);

            emitResult();
        });

    const auto error = job->start(d->signers, content, d->signingMode(d->format));
    if (error.code()) {
        job->deleteLater();
        setError(error.code());
        setErrorText(QString::fromLocal8Bit(error.asString()));
        emitResult();
    }
}

#include "moc_signjob.cpp"
