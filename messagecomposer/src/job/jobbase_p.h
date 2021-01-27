/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_JOBBASE_P_H
#define MESSAGECOMPOSER_JOBBASE_P_H

#include "jobbase.h"

namespace MessageComposer
{
class JobBasePrivate
{
public:
    explicit JobBasePrivate(JobBase *qq)
        : q_ptr(qq)
    {
    }

    virtual ~JobBasePrivate()
    {
    }

    JobBase *q_ptr;
    Q_DECLARE_PUBLIC(JobBase)
};
}

#endif
