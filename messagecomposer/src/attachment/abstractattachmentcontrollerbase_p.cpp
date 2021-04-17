/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 * SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "abstractattachmentcontrollerbase_p.h"
#include "abstractattachmentcontrollerbase.h"

AbstractAttachmentControllerBase::Private::Private(AbstractAttachmentControllerBase *qq)
    : q(qq)
{
}

AbstractAttachmentControllerBase::Private::~Private()
{
}

void AbstractAttachmentControllerBase::Private::attachPublicKeyJobResult(KJob *job)
{
    // The only reason we can't use loadJobResult() and need a separate method
    // is that we want to show the proper caption ("public key" instead of "file")...

    if (job->error()) {
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to attach public key."));
        return;
    }

    Q_ASSERT(dynamic_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job));
    auto ajob = static_cast<MessageComposer::AttachmentFromPublicKeyJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AbstractAttachmentControllerBase::Private::attachVcardFromAddressBook(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get vCard";
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to attach vCard."));
        return;
    }

    auto ajob = static_cast<MessageComposer::AttachmentVcardFromAddressBookJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AbstractAttachmentControllerBase::Private::attachmentRemoved(const AttachmentPart::Ptr &part)
{
    uncompressedParts.remove(part);
}

void AbstractAttachmentControllerBase::Private::compressJobResult(KJob *job)
{
    if (job->error()) {
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to compress attachment"));
        return;
    }

    auto ajob = qobject_cast<AttachmentCompressJob *>(job);
    Q_ASSERT(ajob);
    if (ajob->isCompressedPartLarger()) {
        Q_EMIT q->compressedAttachmentLargerOriginalOccured(job);
        return;
    }
    compressJobOverwriteResult(job);
}

void AbstractAttachmentControllerBase::Private::compressJobOverwriteResult(KJob *job)
{
    auto ajob = qobject_cast<AttachmentCompressJob *>(job);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr compressedPart = ajob->compressedPart();
    qCDebug(MESSAGECOMPOSER_LOG) << "Replacing uncompressed part in model.";
    uncompressedParts[compressedPart] = originalPart;
    bool ok = model->replaceAttachment(originalPart, compressedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Compressed a zombie.";
    }
}

void AbstractAttachmentControllerBase::Private::loadJobResult(KJob *job)
{
    if (job->error()) {
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to attach file"));
        return;
    }

    auto ajob = qobject_cast<AttachmentLoadJob *>(job);
    Q_ASSERT(ajob);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

void AbstractAttachmentControllerBase::Private::openSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : qAsConst(selectedParts)) {
        q->openAttachment(part);
    }
}

void AbstractAttachmentControllerBase::Private::viewSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    for (const AttachmentPart::Ptr &part : qAsConst(selectedParts)) {
        q->viewAttachment(part);
    }
}

void AbstractAttachmentControllerBase::Private::removeSelectedAttachments()
{
    Q_ASSERT(selectedParts.count() >= 1);
    // We must store list, otherwise when we remove it changes selectedParts (as selection changed) => it will crash.
    const AttachmentPart::List toRemove = selectedParts;
    for (const AttachmentPart::Ptr &part : toRemove) {
        model->removeAttachment(part);
    }
}

void AbstractAttachmentControllerBase::Private::reloadAttachment()
{
    Q_ASSERT(selectedParts.count() == 1);
    auto ajob = new AttachmentUpdateJob(selectedParts.constFirst(), q);
    connect(ajob, &AttachmentUpdateJob::result, q, [this](KJob *job) {
        updateJobResult(job);
    });
    ajob->start();
}

void AbstractAttachmentControllerBase::Private::updateJobResult(KJob *job)
{
    if (job->error()) {
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to reload attachment"));
        return;
    }
    auto ajob = qobject_cast<AttachmentUpdateJob *>(job);
    Q_ASSERT(ajob);
    AttachmentPart::Ptr originalPart = ajob->originalPart();
    AttachmentPart::Ptr updatedPart = ajob->updatedPart();

    attachmentRemoved(originalPart);
    bool ok = model->replaceAttachment(originalPart, updatedPart);
    if (!ok) {
        // The attachment was removed from the model while we were compressing.
        qCDebug(MESSAGECOMPOSER_LOG) << "Updated a zombie.";
    }
}
