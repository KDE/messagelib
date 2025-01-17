/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "jobbase.h"

#include "composerjob.h"
#include "jobbase_p.h"

#include "messagecomposer_debug.h"

using namespace MessageComposer;

JobBase::JobBase(QObject *parent)
    : KCompositeJob(parent)
    , d_ptr(new JobBasePrivate(this))
{
}

JobBase::JobBase(JobBasePrivate &dd, QObject *parent)
    : KCompositeJob(parent)
    , d_ptr(&dd)
{
}

JobBase::~JobBase()
{
    delete d_ptr;
}

GlobalPart *JobBase::globalPart()
{
    for (QObject *obj = this; obj != nullptr; obj = obj->parent()) {
        auto composerJob = qobject_cast<ComposerJob *>(obj);
        if (composerJob) {
            return composerJob->globalPart();
        }
    }

    qCCritical(MESSAGECOMPOSER_LOG) << "Job is not part of a Composer.";
    return nullptr;
}

#include "moc_jobbase.cpp"
