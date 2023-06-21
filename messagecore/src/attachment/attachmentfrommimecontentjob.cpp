/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  Based on KMail code by:
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentfrommimecontentjob.h"

#include <KMime/Content>

using namespace MessageCore;
using KMime::Content;

class Q_DECL_HIDDEN MessageCore::AttachmentFromMimeContentJob::AttachmentFromMimeContentJobPrivate
{
public:
    const Content *mMimeContent = nullptr;
};

AttachmentFromMimeContentJob::AttachmentFromMimeContentJob(const Content *content, QObject *parent)
    : AttachmentLoadJob(parent)
    , d(new AttachmentFromMimeContentJobPrivate)
{
    d->mMimeContent = content;
}

AttachmentFromMimeContentJob::~AttachmentFromMimeContentJob() = default;

const Content *AttachmentFromMimeContentJob::mimeContent() const
{
    return d->mMimeContent;
}

void AttachmentFromMimeContentJob::setMimeContent(const Content *content)
{
    d->mMimeContent = content;
}

void AttachmentFromMimeContentJob::doStart()
{
    // Create the AttachmentPart.
    Q_ASSERT(attachmentPart() == nullptr);

    AttachmentPart::Ptr part = AttachmentPart::Ptr(new AttachmentPart);
    auto content = const_cast<Content *>(d->mMimeContent);
    part->setData(content->decodedContent());

    // Get the details from the MIME headers.
    if (auto ct = content->contentType(false)) {
        part->setMimeType(ct->mimeType());
        part->setName(ct->name());
    }

    if (auto ct = content->contentTransferEncoding(false)) {
        part->setEncoding(ct->encoding());
    }

    if (auto ct = content->contentDisposition(false)) {
        part->setFileName(ct->filename());
        part->setInline(ct->disposition() == KMime::Headers::CDinline);
    }

    if (auto ct = content->contentDescription(false)) {
        part->setDescription(ct->asUnicodeString());
    }

    setAttachmentPart(part);
    emitResult(); // Success.
}

#include "moc_attachmentfrommimecontentjob.cpp"
