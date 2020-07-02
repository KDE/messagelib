/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONVERTSNIPPETVARIABLESUTIL_H
#define CONVERTSNIPPETVARIABLESUTIL_H
#include "messagecomposer_export.h"
#include <QString>

namespace MessageComposer {
namespace ConvertSnippetVariablesUtil {
enum VariableType {
    CcAddr,
    CcLname,
    CcFname,
    CcName,
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
    YearLastMonth
};

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);
}
}
#endif // CONVERTSNIPPETVARIABLESUTIL_H
