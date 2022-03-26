/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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

    virtual ~JobBasePrivate() = default;

    JobBase *q_ptr;
    Q_DECLARE_PUBLIC(JobBase)
};
}
