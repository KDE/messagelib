/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "contentjobbase.h"
#include "contentjobbase_p.h"

#include "messagecomposer_debug.h"

#include <KMime/Content>

using namespace MessageComposer;

void ContentJobBasePrivate::init(QObject *parent)
{
    Q_Q(ContentJobBase);
    auto parentJob = qobject_cast<ContentJobBase *>(parent);
    if (parentJob) {
        if (!parentJob->appendSubjob(q)) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Impossible to add subjob.";
        }
    }
}

void ContentJobBasePrivate::doNextSubjob()
{
    Q_Q(ContentJobBase);
    if (q->hasSubjobs()) {
        q->subjobs().first()->start();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Calling process.";
        q->process();
    }
}

ContentJobBase::ContentJobBase(QObject *parent)
    : JobBase(*new ContentJobBasePrivate(this), parent)
{
    Q_D(ContentJobBase);
    d->init(parent);
}

ContentJobBase::ContentJobBase(ContentJobBasePrivate &dd, QObject *parent)
    : JobBase(dd, parent)
{
    Q_D(ContentJobBase);
    d->init(parent);
}

ContentJobBase::~ContentJobBase() = default;

void ContentJobBase::start()
{
    doStart();
}

KMime::Content *ContentJobBase::content() const
{
    Q_D(const ContentJobBase);
    // Q_ASSERT( !hasSubjobs() ); // Finished. // JobBase::hasSubjobs is not const :-/ TODO const_cast??
    Q_ASSERT(d->resultContent); // process() should do something.
    return d->resultContent;
}

bool ContentJobBase::appendSubjob(ContentJobBase *job)
{
    job->setParent(this);
    return KCompositeJob::addSubjob(job);
}

void ContentJobBase::setExtraContent(KMime::Content *extra)
{
    Q_D(ContentJobBase);

    d->extraContent = extra;
}

KMime::Content *ContentJobBase::extraContent() const
{
    Q_D(const ContentJobBase);

    return d->extraContent;
}

bool ContentJobBase::addSubjob(KJob *job)
{
    Q_UNUSED(job)
    qCCritical(MESSAGECOMPOSER_LOG) << "Use appendJob() instead.";
    Q_ASSERT(false);
    return false;
}

void ContentJobBase::doStart()
{
    Q_D(ContentJobBase);
    Q_ASSERT(d->resultContent == nullptr && d->subjobContents.isEmpty()); // Not started.
    Q_ASSERT(!error()); // Jobs emitting an error in doStart should not call ContentJobBase::doStart().
    d->doNextSubjob();
}

void ContentJobBase::slotResult(KJob *job)
{
    Q_D(ContentJobBase);
    KCompositeJob::slotResult(job); // Handles errors and removes subjob.
    qCDebug(MESSAGECOMPOSER_LOG) << "A subjob finished." << subjobs().count() << "more to go.";
    if (error()) {
        return;
    }

    Q_ASSERT(dynamic_cast<ContentJobBase *>(job));
    auto cjob = static_cast<ContentJobBase *>(job);
    d->subjobContents.append(cjob->content());
    d->doNextSubjob();
}

#include "moc_contentjobbase.cpp"
