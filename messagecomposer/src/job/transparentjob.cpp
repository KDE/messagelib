/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/transparentjob.h"

#include "contentjobbase_p.h"

#include "messagecomposer_debug.h"
#include <kmime/kmime_content.h>
#include <kmime/kmime_message.h>
using namespace MessageComposer;

class MessageComposer::TransparentJobPrivate : public MessageComposer::ContentJobBasePrivate
{
public:
    TransparentJobPrivate(TransparentJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    KMime::Content *content = nullptr;

    Q_DECLARE_PUBLIC(TransparentJob)
};

TransparentJob::TransparentJob(QObject *parent)
    : MessageComposer::ContentJobBase(*new TransparentJobPrivate(this), parent)
{
}

TransparentJob::~TransparentJob()
{
}

void TransparentJob::setContent(KMime::Content *content)
{
    Q_D(TransparentJob);

    d->content = content;
}

void TransparentJob::process()
{
    Q_D(TransparentJob);
    d->resultContent = d->content;
    emitResult();
}
