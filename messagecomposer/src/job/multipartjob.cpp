/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/multipartjob.h"
#include "contentjobbase_p.h"

#include "messagecomposer_debug.h"

#include <KMime/Content>

using namespace MessageComposer;

class MessageComposer::MultipartJobPrivate : public ContentJobBasePrivate
{
public:
    MultipartJobPrivate(MultipartJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    QByteArray subtype;
};

MultipartJob::MultipartJob(QObject *parent)
    : ContentJobBase(*new MultipartJobPrivate(this), parent)
{
}

MultipartJob::~MultipartJob() = default;

QByteArray MultipartJob::multipartSubtype() const
{
    Q_D(const MultipartJob);
    return d->subtype;
}

void MultipartJob::setMultipartSubtype(const QByteArray &subtype)
{
    Q_D(MultipartJob);
    d->subtype = subtype;
}

void MultipartJob::process()
{
    Q_D(MultipartJob);
    Q_ASSERT(d->resultContent == nullptr); // Not processed before.
    Q_ASSERT(!d->subtype.isEmpty());
    d->resultContent = new KMime::Content;
    auto contentType = d->resultContent->contentType(true);
    contentType->setMimeType("multipart/" + d->subtype);
    contentType->setBoundary(KMime::multiPartBoundary());
    d->resultContent->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    d->resultContent->setPreamble("This is a multi-part message in MIME format.\n");
    for (KMime::Content *c : std::as_const(d->subjobContents)) {
        d->resultContent->appendContent(c);
        if (c->contentTransferEncoding()->encoding() == KMime::Headers::CE8Bit) {
            d->resultContent->contentTransferEncoding()->setEncoding(KMime::Headers::CE8Bit);
            break;
        }
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "Created" << d->resultContent->contentType()->mimeType() << "content with" << d->resultContent->contents().count()
                                 << "subjobContents.";
    emitResult();
}

#include "moc_multipartjob.cpp"
