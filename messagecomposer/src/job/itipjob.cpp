/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/itipjob.h"

#include "contentjobbase_p.h"
#include "job/multipartjob.h"
#include "job/singlepartjob.h"
#include "part/itippart.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <KMime/Content>

using namespace MessageComposer;

class MessageComposer::ItipJobPrivate : public ContentJobBasePrivate
{
public:
    ItipJobPrivate(ItipJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    ContentJobBase *createStandardItipJob();
    SinglepartJob *createOutlookItipJob();
    SinglepartJob *createInvitationBodyJob();
    SinglepartJob *createInvitationJob();

    ItipPart *itipPart = nullptr;

    Q_DECLARE_PUBLIC(ItipJob)
};

SinglepartJob *ItipJobPrivate::createInvitationBodyJob()
{
    auto job = new SinglepartJob; // No parent.
    job->contentType()->setMimeType("text/plain");
    job->contentType()->setCharset("utf-8");
    job->contentType()->setParameter(QByteArrayLiteral("method"), itipPart->method());
    job->contentTransferEncoding()->setEncoding(KMime::Headers::CEquPr);
    job->contentDisposition()->setDisposition(KMime::Headers::CDinline);
    job->setData(KMime::CRLFtoLF(itipPart->invitationBody().toUtf8()));
    return job;
}

SinglepartJob *ItipJobPrivate::createInvitationJob()
{
    auto job = new SinglepartJob;
    job->contentType()->setMimeType("text/calendar");
    job->contentType()->setCharset("utf-8");
    job->contentType()->setName(QStringLiteral("cal.ics"));
    job->contentType()->setParameter(QByteArrayLiteral("method"), itipPart->method());
    job->contentDisposition()->setDisposition(KMime::Headers::CDattachment);
    job->contentTransferEncoding()->setEncoding(KMime::Headers::CEquPr);
    job->setData(KMime::CRLFtoLF(itipPart->invitation().toUtf8()));
    return job;
}

ContentJobBase *ItipJobPrivate::createStandardItipJob()
{
    auto bodyJob = createInvitationBodyJob();
    if (itipPart->invitation().isEmpty()) {
        return bodyJob;
    }

    auto mpJob = new MultipartJob;
    mpJob->setMultipartSubtype("mixed");
    mpJob->appendSubjob(bodyJob);
    mpJob->appendSubjob(createInvitationJob());
    return mpJob;
}

SinglepartJob *ItipJobPrivate::createOutlookItipJob()
{
    auto job = new SinglepartJob;
    job->contentType()->setMimeType("text/calendar");
    job->contentType()->setName(QStringLiteral("cal.ics"));
    job->contentType()->setParameter(QByteArrayLiteral("method"), QStringLiteral("request"));
    job->contentType()->setCharset("utf-8");

    if (!itipPart->invitation().isEmpty()) {
        job->contentDisposition()->setDisposition(KMime::Headers::CDinline);
        job->contentTransferEncoding()->setEncoding(KMime::Headers::CEquPr);
        job->setData(KMime::CRLFtoLF(itipPart->invitation().toUtf8()));
    }

    return job;
}

ItipJob::ItipJob(ItipPart *itipPart, QObject *parent)
    : ContentJobBase(*new ItipJobPrivate(this), parent)
{
    Q_D(ItipJob);
    d->itipPart = itipPart;
}

ItipJob::~ItipJob() = default;

ItipPart *ItipJob::itipPart() const
{
    Q_D(const ItipJob);
    return d->itipPart;
}

void ItipJob::setItipPart(ItipPart *part)
{
    Q_D(ItipJob);
    d->itipPart = part;
}

void ItipJob::doStart()
{
    Q_D(ItipJob);
    Q_ASSERT(d->itipPart);

    if (d->itipPart->outlookConformInvitation()) {
        appendSubjob(d->createOutlookItipJob());
    } else {
        appendSubjob(d->createStandardItipJob());
    }

    ContentJobBase::doStart();
}

void ItipJob::process()
{
    Q_D(ItipJob);
    // The content has been created by our subjob.
    Q_ASSERT(d->subjobContents.count() == 1);
    d->resultContent = d->subjobContents.constFirst();
    emitResult();
}

#include "moc_itipjob.cpp"
