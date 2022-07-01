/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <QList>

#include <KCompositeJob>

namespace MessageComposer
{
class GlobalPart;
class JobBasePrivate;

/**
  A dummy abstract class defining some errors pertaining to the Composer.
  It is meant to be subclassed.
*/
class MESSAGECOMPOSER_EXPORT JobBase : public KCompositeJob
{
    Q_OBJECT

public:
    using List = QList<JobBase *>;

    enum Error {
        BugError = UserDefinedError + 1,
        IncompleteError,
        UserCancelledError,
        UserError = UserDefinedError + 42,
    };

    explicit JobBase(QObject *parent = nullptr);
    ~JobBase() override;

    // asserts if no Composer parent
    GlobalPart *globalPart();

protected:
    JobBasePrivate *const d_ptr;
    JobBase(JobBasePrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(JobBase)
};
} // namespace MessageComposer
