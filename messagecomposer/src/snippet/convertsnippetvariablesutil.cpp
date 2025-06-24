/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesutil.h"
using namespace Qt::Literals::StringLiterals;

#include <QDateTime>
#include <QLocale>

QString MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type)
{
    switch (type) {
    case CcAddr:
        return u"%CCADDR"_s;
    case CcLname:
        return u"%CCLNAME"_s;
    case CcFname:
        return u"%CCFNAME"_s;
    case CcName:
        return u"%CCNAME"_s;
    case BccAddr:
        return u"%BCCADDR"_s;
    case BccLname:
        return u"%BCCLNAME"_s;
    case BccFname:
        return u"%BCCFNAME"_s;
    case BccName:
        return u"%BCCNAME"_s;
    case FullSubject:
        return u"%FULLSUBJECT"_s;
    case ToAddr:
        return u"%TOADDR"_s;
    case ToFname:
        return u"%TOFNAME"_s;
    case ToLname:
        return u"%TOLNAME"_s;
    case ToName:
        return u"%TONAME"_s;
    case FromAddr:
        return u"%FROMADDR"_s;
    case FromLname:
        return u"%FROMLNAME"_s;
    case FromFname:
        return u"%FROMFNAME"_s;
    case FromName:
        return u"%FROMNAME"_s;
    case Dow:
        return u"%DOW"_s;
    case Date:
        return u"%DATE"_s;
    case ShortDate:
        return u"%SHORTDATE"_s;
    case Time:
        return u"%TIME"_s;
    case TimeLong:
        return u"%TIMELONG"_s;
    case AttachmentCount:
        return u"%ATTACHMENTCOUNT"_s;
    case AttachmentName:
        return u"%ATTACHMENTNAMES"_s;
    case AttachmentFilenames:
        return u"%ATTACHMENTFILENAMES"_s;
    case AttachmentNamesAndSizes:
        return u"%ATTACHMENTNAMESANDSIZES"_s;
    case Year:
        return u"%YEAR"_s;
    case LastYear:
        return u"%LASTYEAR"_s;
    case NextYear:
        return u"%NEXTYEAR"_s;
    case MonthNumber:
        return u"%MONTHNUMBER"_s; // 1-12
    case DayNumber:
        return u"%DAYNUMBER"_s; // 1-28 to 31
    case CustomDate:
        return u"%CUSTOMDATE"_s; // dd MMM yyyy
    case DayOfMonth:
        return u"%DAYOFMONTH"_s; // 1-31
    case WeekNumber:
        return u"%WEEKNUMBER"_s; // 1-52
    case MonthNameShort:
        return u"%MONTHNAMESHORT"_s; //(Jan-Dec)
    case MonthNameLong:
        return u"%MONTHNAMELONG"_s; //(January-December)
    case DayOfWeek:
        return u"%DAYOFWEEK"_s; //(1-7)
    case DayOfWeekNameShort:
        return u"%DAYOFWEEKNAMESHORT"_s; //(Mon-Sun)
    case DayOfWeekNameLong:
        return u"%DAYOFWEEKNAMELONG"_s; //(Monday-Sunday)
    case YearLastMonth:
        return u"%YEARLASTMONTH"_s; // january 2020 will how 2019-12
    case LastMonthNameLong:
        return u"%LASTMONTHNAMELONG"_s;
    }
    return {};
}

QString MessageComposer::ConvertSnippetVariablesUtil::shortDate()
{
    QLocale locale;
    return locale.toString(QDate::currentDate(), QLocale::ShortFormat);
}

QString MessageComposer::ConvertSnippetVariablesUtil::longDate()
{
    QLocale locale;
    return locale.toString(QDate::currentDate(), QLocale::LongFormat);
}

QString MessageComposer::ConvertSnippetVariablesUtil::customDate()
{
    QLocale locale;
    return locale.toString(QDate::currentDate(), u"dd MMM yyyy"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::shortTime()
{
    QLocale locale;
    return locale.toString(QTime::currentTime(), QLocale::ShortFormat);
}

QString MessageComposer::ConvertSnippetVariablesUtil::longTime()
{
    QLocale locale;
    return locale.toString(QTime::currentTime(), QLocale::LongFormat);
}

QString MessageComposer::ConvertSnippetVariablesUtil::insertDayOfWeek()
{
    const QDateTime date = QDateTime::currentDateTime().toLocalTime();
    const QString str = QLocale().dayName(date.date().dayOfWeek(), QLocale::LongFormat);
    return str;
}

QString MessageComposer::ConvertSnippetVariablesUtil::lastYear()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.year() - 1);
}

QString MessageComposer::ConvertSnippetVariablesUtil::nextYear()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.year() + 1);
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayNumber()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.day());
}

QString MessageComposer::ConvertSnippetVariablesUtil::monthNumber()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.month());
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfMonth()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.daysInMonth());
}

QString MessageComposer::ConvertSnippetVariablesUtil::weekNumber()
{
    const QDate date = QDate::currentDate();
    return QString::number(date.weekNumber());
}

QString MessageComposer::ConvertSnippetVariablesUtil::monthNameShort()
{
    const QDate date = QDate::currentDate();
    return date.toString(u"MMM"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::monthNameLong()
{
    const QDate date = QDate::currentDate();
    return date.toString(u"MMMM"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::lastMonthNameLong()
{
    const QDate date = QDate::currentDate().addMonths(-1);
    return date.toString(u"MMMM"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameShort()
{
    const QDate date = QDate::currentDate();
    return date.toString(u"ddd"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong()
{
    const QDate date = QDate::currentDate();
    return date.toString(u"dddd"_s);
}

QString MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth()
{
    const QDate date = QDate::currentDate();
    return date.toString(date.toString(u"yyyy-MMM"_s));
}

QString MessageComposer::ConvertSnippetVariablesUtil::year()
{
    const QDate date = QDate::currentDate();
    return date.toString(QString::number(date.year()));
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfWeek()
{
    const QDate date = QDate::currentDate();
    return date.toString(QString::number(date.dayOfWeek()));
}
