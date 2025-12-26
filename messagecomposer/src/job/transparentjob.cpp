/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/transparentjob.h"

#include "contentjobbase_p.h"

#include <KMime/Content>
#include <KMime/Message>
using namespace MessageComposer;

class MessageComposer::TransparentJobPrivate : public MessageComposer::ContentJobBasePrivate
{
public:
    explicit TransparentJobPrivate(TransparentJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    std::unique_ptr<KMime::Content> content;

    Q_DECLARE_PUBLIC(TransparentJob)
};

TransparentJob::TransparentJob(QObject *parent)
    : MessageComposer::ContentJobBase(*new TransparentJobPrivate(this), parent)
{
}

TransparentJob::~TransparentJob() = default;

void TransparentJob::setContent(std::unique_ptr<KMime::Content> &&content)
{
    Q_D(TransparentJob);

    d->content = std::move(content);
}

void TransparentJob::process()
{
    Q_D(TransparentJob);
    d->resultContent = std::move(d->content);
    emitResult();
}

#include "moc_transparentjob.cpp"
