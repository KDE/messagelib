/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Parts based on KMail code by:
  Various authors.

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/attachmentjob.h"
#include "contentjobbase_p.h"
#include "job/singlepartjob.h"
#include "part/globalpart.h"
#include "utils/util.h"

#include "messagecomposer_debug.h"

using namespace MessageComposer;
using namespace MessageCore;

class MessageComposer::AttachmentJobPrivate : public ContentJobBasePrivate
{
public:
    AttachmentJobPrivate(AttachmentJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    AttachmentPart::Ptr part;

    Q_DECLARE_PUBLIC(AttachmentJob)
};

AttachmentJob::AttachmentJob(AttachmentPart::Ptr part, QObject *parent)
    : ContentJobBase(*new AttachmentJobPrivate(this), parent)
{
    Q_D(AttachmentJob);
    d->part = part;
}

AttachmentJob::~AttachmentJob() = default;

AttachmentPart::Ptr AttachmentJob::attachmentPart() const
{
    Q_D(const AttachmentJob);
    return d->part;
}

void AttachmentJob::setAttachmentPart(const AttachmentPart::Ptr &part)
{
    Q_D(AttachmentJob);
    d->part = part;
}

void AttachmentJob::doStart()
{
    Q_D(AttachmentJob);
    Q_ASSERT(d->part);

    if (d->part->mimeType() == "multipart/digest" || d->part->mimeType() == "message/rfc822") {
        // this is actually a digest, so we don't want any additional headers
        // the attachment is really a complete multipart/digest subtype
        // and us adding our own headers would break it. so copy over the content
        // and leave it alone
        auto part = new KMime::Content;
        part->setContent(d->part->data());
        part->parse();
        d->subjobContents << part;
        process();
        return;
    }

    // Set up a subjob to generate the attachment content.
    auto sjob = new SinglepartJob(this);
    sjob->setData(d->part->data());

    // Set up the headers.
    // rfc822 forwarded messages have 7bit CTE, the message itself will have
    //  its own CTE for the content
    if (d->part->mimeType() == "message/rfc822") {
        sjob->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    } else {
        sjob->contentTransferEncoding()->setEncoding(d->part->encoding());
    }

    auto ct = sjob->contentType();
    ct->setMimeType(d->part->mimeType()); // setMimeType() clears all other params.
    ct->setName(d->part->name());
    if (ct->isText()) {
        // If it is a text file, detect its charset.
        // sjob->contentType()->setCharset( d->detectCharset( d->part->data() ) );

        // From my few tests, this is *very* unreliable.
        // Therefore, if we do not know which charset to use, just use UTF-8.
        // (cberzan)
        QByteArray textCharset = d->part->charset();
        if (textCharset.isEmpty()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "No charset specified. Using UTF-8.";
            textCharset = "utf-8";
        }
        ct->setCharset(textCharset);
    }

    sjob->contentDescription()->fromUnicodeString(d->part->description());

    auto contentDisposition = sjob->contentDisposition();
    contentDisposition->setFilename(d->part->fileName());
    contentDisposition->setRFC2047Charset("utf-8");
    if (d->part->isInline()) {
        contentDisposition->setDisposition(KMime::Headers::CDinline);
    } else {
        contentDisposition->setDisposition(KMime::Headers::CDattachment);
    }

    ContentJobBase::doStart();
}

void AttachmentJob::process()
{
    Q_D(AttachmentJob);
    // The content has been created by our subjob.
    Q_ASSERT(d->subjobContents.count() == 1);
    d->resultContent = d->subjobContents.constFirst();
    emitResult();
}

#include "moc_attachmentjob.cpp"
