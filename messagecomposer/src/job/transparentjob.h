/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "contentjobbase.h"
#include "messagecomposer_export.h"

namespace MessageComposer
{
class TransparentJobPrivate;

/**
  A job that just wraps some KMime::Content into a job object
  for use as a subjob in another job.
 */
class MESSAGECOMPOSER_EXPORT TransparentJob : public MessageComposer::ContentJobBase
{
    Q_OBJECT

public:
    explicit TransparentJob(QObject *parent = nullptr);
    ~TransparentJob() override;

    void setContent(KMime::Content *content);
    void process() override;

private:
    Q_DECLARE_PRIVATE(TransparentJob)
};
}
