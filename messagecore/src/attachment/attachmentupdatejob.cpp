/*
   Copyright (C) 2014-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "attachmentfromfolderjob.h"
#include "MessageCore/AttachmentFromUrlBaseJob"
#include "attachmentfromurljob.h"
#include "attachmentupdatejob.h"
#include <QUrl>
#include "attachmentfromurlutils.h"
#include "messagecore_debug.h"
#include <QTimer>
#include <KLocalizedString>

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
    connect(job, SIGNAL(result(KJob*)), q, SLOT(loadJobResult(KJob*)));
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
    AttachmentLoadJob *ajob = static_cast<AttachmentLoadJob *>(job);
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
    QTimer::singleShot(0, this, SLOT(doStart()));
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
