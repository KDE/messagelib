/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentupdatejob.h"
#include "MessageCore/AttachmentFromUrlBaseJob"
#include "attachmentfromfolderjob.h"
#include "attachmentfromurljob.h"
#include "attachmentfromurlutils.h"
#include "messagecore_debug.h"
#include <KLocalizedString>
#include <QTimer>

using namespace MessageCore;

class Q_DECL_HIDDEN MessageCore::AttachmentUpdateJob::Private
{
public:
    Private(AttachmentUpdateJob *qq);

    void doStart(); // slot
    void loadJobResult(KJob *);

    AttachmentUpdateJob *const q;
    AttachmentPart::Ptr mOriginalPart;
    AttachmentPart::Ptr mUpdatedPart;
};

AttachmentUpdateJob::Private::Private(AttachmentUpdateJob *qq)
    : q(qq)
{
}

void AttachmentUpdateJob::Private::doStart()
{
    Q_ASSERT(mOriginalPart);
    if (mOriginalPart->url().isEmpty()) {
        qCDebug(MESSAGECORE_LOG) << " url is empty. We can't update file";
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("URL is empty."));
        q->emitResult();
        return;
    }
    MessageCore::AttachmentFromUrlBaseJob *job = MessageCore::AttachmentFromUrlUtils::createAttachmentJob(mOriginalPart->url(), q);
    connect(job, &AttachmentFromUrlBaseJob::result, q, [this](KJob *job) {
        loadJobResult(job);
    });
    job->start();
}

void AttachmentUpdateJob::Private::loadJobResult(KJob *job)
{
    if (job->error()) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(job->errorString());
        q->emitResult();
        return;
    }

    Q_ASSERT(dynamic_cast<AttachmentLoadJob *>(job));
    auto ajob = static_cast<AttachmentLoadJob *>(job);
    mUpdatedPart = ajob->attachmentPart();
    mUpdatedPart->setName(q->originalPart()->name());
    mUpdatedPart->setFileName(q->originalPart()->fileName());
    mUpdatedPart->setDescription(q->originalPart()->description());
    mUpdatedPart->setSigned(q->originalPart()->isSigned());
    mUpdatedPart->setEncrypted(q->originalPart()->isEncrypted());
    mUpdatedPart->setEncoding(q->originalPart()->encoding());
    mUpdatedPart->setMimeType(q->originalPart()->mimeType());
    mUpdatedPart->setInline(q->originalPart()->isInline());
    q->emitResult(); // Success.
}

AttachmentUpdateJob::AttachmentUpdateJob(const AttachmentPart::Ptr &part, QObject *parent)
    : KJob(parent)
    , d(new Private(this))
{
    d->mOriginalPart = part;
}

AttachmentUpdateJob::~AttachmentUpdateJob()
{
    delete d;
}

void AttachmentUpdateJob::start()
{
    QTimer::singleShot(0, this, [this]() {
        d->doStart();
    });
}

AttachmentPart::Ptr AttachmentUpdateJob::originalPart() const
{
    return d->mOriginalPart;
}

AttachmentPart::Ptr AttachmentUpdateJob::updatedPart() const
{
    return d->mUpdatedPart;
}

#include "moc_attachmentupdatejob.cpp"
