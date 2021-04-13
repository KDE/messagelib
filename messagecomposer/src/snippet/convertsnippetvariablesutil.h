/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <QString>

namespace MessageComposer
{
namespace ConvertSnippetVariablesUtil
{
enum VariableType {
    CcAddr,
    CcLname,
    CcFname,
    CcName,
    BccAddr,
    BccLname,
    BccFname,
    BccName,

    FullSubject,
    ToAddr,
    ToFname,
    ToLname,
    ToName,
    FromAddr,
    FromLname,
    FromFname,
    FromName,
    Dow,
    AttachmentName,
    Date,
    ShortDate,
    Time,
    TimeLong,
    AttachmentCount,
    AttachmentFilenames,
    AttachmentNamesAndSizes,
    Year,
    LastYear,
    NextYear,
    MonthNumber,
    DayOfMonth,
    WeekNumber,
    MonthNameShort,
    MonthNameLong,
    DayOfWeek,
    DayOfWeekNameShort,
    DayOfWeekNameLong,
    YearLastMonth,
    LastMonthNameLong
};

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString shortDate();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString longDate();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString shortTime();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString longTime();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString insertDayOfWeek();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString lastYear();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString nextYear();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString monthNumber();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString dayOfMonth();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString weekNumber();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString monthNameShort();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString monthNameLong();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString dayOfWeekNameShort();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString dayOfWeekNameLong();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString yearLastMonth();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString year();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString dayOfWeek();
Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString lastMonthNameLong();
}
}
