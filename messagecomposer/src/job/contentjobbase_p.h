/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONTENTJOBBASE_P_H
#define CONTENTJOBBASE_P_H

#include "contentjobbase.h"
#include "job/jobbase_p.h"

#include <kmime/kmime_content.h>

namespace MessageComposer {
class ContentJobBasePrivate : public JobBasePrivate
{
public:
    explicit ContentJobBasePrivate(ContentJobBase *qq)
        : JobBasePrivate(qq)
    {
    }

    void init(QObject *parent);
    void doNextSubjob();

    KMime::Content::List subjobContents;
    KMime::Content *resultContent = nullptr;
    KMime::Content *extraContent = nullptr;

    Q_DECLARE_PUBLIC(ContentJobBase)
};
}

#endif
