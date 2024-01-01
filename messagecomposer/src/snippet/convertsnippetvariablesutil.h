/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

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
    LastMonthNameLong,
};

[[nodiscard]] MESSAGECOMPOSER_EXPORT QString snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);

[[nodiscard]] MESSAGECOMPOSER_EXPORT QString shortDate();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString longDate();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString shortTime();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString longTime();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString insertDayOfWeek();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString lastYear();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString nextYear();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNumber();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfMonth();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString weekNumber();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNameShort();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNameLong();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeekNameShort();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeekNameLong();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString yearLastMonth();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString year();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeek();
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString lastMonthNameLong();
}
}
