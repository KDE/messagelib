/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "convertsnippetvariablesutil.h"

QString MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type)
{
    switch (type) {
    case CcAddr:
        return QStringLiteral("%CCADDR");
    case CcLname:
        return QStringLiteral("%CCLNAME");
    case CcFname:
        return QStringLiteral("%CCFNAME");
    case CcName:
        return QStringLiteral("%CCNAME");
    case FullSubject:
        return QStringLiteral("%FULLSUBJECT");
    case ToAddr:
        return QStringLiteral("%TOADDR");
    case ToFname:
        return QStringLiteral("%TOFNAME");
    case ToLname:
        return QStringLiteral("%TOLNAME");
    case ToName:
        return QStringLiteral("%TONAME");
    case FromAddr:
        return QStringLiteral("%FROMADDR");
    case FromLname:
        return QStringLiteral("%FROMLNAME");
    case FromFname:
        return QStringLiteral("%FROMFNAME");
    case FromName:
        return QStringLiteral("%FROMNAME");
    case Dow:
        return QStringLiteral("%DOW");
    case Date:
        return QStringLiteral("%DATE");
    case ShortDate:
        return QStringLiteral("%SHORTDATE");
    case Time:
        return QStringLiteral("%TIME");
    case TimeLong:
        return QStringLiteral("%TIMELONG");
    case AttachmentCount:
        return QStringLiteral("%ATTACHMENTCOUNT");
    case AttachmentName:
        return QStringLiteral("%ATTACHMENTNAMES");
    case AttachmentFilenames:
        return QStringLiteral("%ATTACHMENTFILENAMES");
    case AttachmentNamesAndSizes:
        return QStringLiteral("%ATTACHMENTNAMESANDSIZES");
    case Year:
        return QStringLiteral("%YEAR");
    case LastYear:
        return QStringLiteral("%LASTYEAR");
    case NextYear:
        return QStringLiteral("%NEXTYEAR");
    case MonthNumber:
        return QStringLiteral("%MONTHNUMBER"); //1-12
    case DayOfMonth:
        return QStringLiteral("%DAYOFMONTH"); //1-31
    case WeekNumber:
        return QStringLiteral("%WEEKNUMBER"); //1-52
    case MonthNameShort:
        return QStringLiteral("%MONTHNAMESHORT"); //(Jan-Dec)
    case MonthNameLong:
        return QStringLiteral("%MONTHNAMELONG"); //(January-December)
    case DayOfWeek:
        return QStringLiteral("%DAYOFWEEK"); //(1-7)
    case DayOfWeekNameShort:
        return QStringLiteral("%DAYOFWEEKNAMESHORT"); //(Mon-Sun)
    case DayOfWeekNameLong:
        return QStringLiteral("%DAYOFWEEKNAMELONG"); //(Monday-Sunday)
    case YearLastMonth:
        return QStringLiteral("%YEARLASTMONTH"); //january 2020 will how 2019-12
    }
    return {};
}
