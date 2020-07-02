/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentloadjob.h"

#include <QTimer>

using namespace MessageCore;

class Q_DECL_HIDDEN MessageCore::AttachmentLoadJob::Private
{
public:
    AttachmentPart::Ptr mPart;
};

AttachmentLoadJob::AttachmentLoadJob(QObject *parent)
    : KJob(parent)
    , d(new Private)
{
}

AttachmentLoadJob::~AttachmentLoadJob()
{
    delete d;
}

void AttachmentLoadJob::start()
{
    QTimer::singleShot(0, this, &AttachmentLoadJob::doStart);
}

AttachmentPart::Ptr AttachmentLoadJob::attachmentPart() const
{
    return d->mPart;
}

void AttachmentLoadJob::setAttachmentPart(const AttachmentPart::Ptr &part)
{
    d->mPart = part;
}
