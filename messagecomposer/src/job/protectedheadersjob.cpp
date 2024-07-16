/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/protectedheadersjob.h"

#include "contentjobbase_p.h"
#include "job/singlepartjob.h"

#include <KMime/Content>
#include <KMime/Message>

using namespace MessageComposer;

class MessageComposer::ProtectedHeadersJobPrivate : public ContentJobBasePrivate
{
public:
    ProtectedHeadersJobPrivate(ProtectedHeadersJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    KMime::Content *content = nullptr;
    KMime::Message *skeletonMessage = nullptr;

    bool obvoscate = false;

    Q_DECLARE_PUBLIC(ProtectedHeadersJob)
};

ProtectedHeadersJob::ProtectedHeadersJob(QObject *parent)
    : ContentJobBase(*new ProtectedHeadersJobPrivate(this), parent)
{
}

ProtectedHeadersJob::~ProtectedHeadersJob() = default;

void ProtectedHeadersJob::setContent(KMime::Content *content)
{
    Q_D(ProtectedHeadersJob);

    d->content = content;
    if (content) {
        d->content->assemble();
    }
}

void ProtectedHeadersJob::setSkeletonMessage(KMime::Message *skeletonMessage)
{
    Q_D(ProtectedHeadersJob);

    d->skeletonMessage = skeletonMessage;
}

void ProtectedHeadersJob::setObvoscate(bool obvoscate)
{
    Q_D(ProtectedHeadersJob);

    d->obvoscate = obvoscate;
}

void ProtectedHeadersJob::doStart()
{
    Q_D(ProtectedHeadersJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.
    Q_ASSERT(d->skeletonMessage); // We need a skeletonMessage to proceed

    auto subject = d->skeletonMessage->header<KMime::Headers::Subject>();
    if (d->obvoscate && subject) {
        // Create protected header lagacy mimepart with replaced headers
        auto cjob = new SinglepartJob;
        auto ct = cjob->contentType();
        ct->setMimeType("text/plain");
        ct->setCharset(subject->rfc2047Charset());
        ct->setParameter(QByteArrayLiteral("protected-headers"), QStringLiteral("v1"));
        cjob->contentDisposition()->setDisposition(KMime::Headers::contentDisposition::CDinline);
        cjob->setData(subject->type() + QByteArray(": ") + subject->asUnicodeString().toUtf8());

        QObject::connect(cjob, &SinglepartJob::finished, this, [d, cjob]() {
            auto mixedPart = new KMime::Content();
            const QByteArray boundary = KMime::multiPartBoundary();
            mixedPart->contentType()->setMimeType("multipart/mixed");
            mixedPart->contentType(false)->setBoundary(boundary);
            mixedPart->appendContent(cjob->content());

            // if setContent hasn't been called, we assume that a subjob was added
            // and we want to use that
            if (!d->content || !d->content->hasContent()) {
                Q_ASSERT(d->subjobContents.size() == 1);
                d->content = d->subjobContents.constFirst();
            }

            mixedPart->appendContent(d->content);
            d->content = mixedPart;
        });
        appendSubjob(cjob);
    }

    ContentJobBase::doStart();
}

void ProtectedHeadersJob::process()
{
    Q_D(ProtectedHeadersJob);

    // if setContent hasn't been called, we assume that a subjob was added
    // and we want to use that
    if (!d->content || !d->content->hasContent()) {
        Q_ASSERT(d->subjobContents.size() == 1);
        d->content = d->subjobContents.constFirst();
    }

    auto subject = d->skeletonMessage->header<KMime::Headers::Subject>();
    const auto headers = d->skeletonMessage->headers();
    for (const auto &header : headers) {
        const QByteArray headerType(header->type());
        if (headerType.startsWith("X-KMail-")) {
            continue;
        }
        if (headerType == "Bcc") {
            continue;
        }
        if (headerType.startsWith("Content-")) {
            continue;
        }
        // A workaround for #439958
        // KMime strips sometimes the newlines from long headers, if those
        // headers are in the signature block, this breaks the signature.
        // The simplest workaround is not to sign those headers until this
        // get fixed in KMime.
        if (header->as7BitString().length() > 70) {
            continue;
        }
        auto copyHeader = KMime::Headers::createHeader(headerType);
        if (!copyHeader) {
            copyHeader = new KMime::Headers::Generic(headerType.constData(), headerType.size());
        }
        copyHeader->from7BitString(header->as7BitString(false));
        d->content->appendHeader(copyHeader);
    }

    if (d->obvoscate && subject) {
        subject->clear();
        subject->from7BitString("...");
    }
    auto contentType = d->content->header<KMime::Headers::ContentType>();
    contentType->setParameter(QByteArrayLiteral("protected-headers"), QStringLiteral("v1"));

    d->resultContent = d->content;

    emitResult();
}

#include "moc_protectedheadersjob.cpp"
