/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesutil.h"

#include <QDateTime>
#include <QLocale>

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
    case BccAddr:
        return QStringLiteral("%BCCADDR");
    case BccLname:
        return QStringLiteral("%BCCLNAME");
    case BccFname:
        return QStringLiteral("%BCCFNAME");
    case BccName:
        return QStringLiteral("%BCCNAME");
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
        return QStringLiteral("%MONTHNUMBER"); // 1-12
    case DayOfMonth:
        return QStringLiteral("%DAYOFMONTH"); // 1-31
    case WeekNumber:
        return QStringLiteral("%WEEKNUMBER"); // 1-52
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
        return QStringLiteral("%YEARLASTMONTH"); // january 2020 will how 2019-12
    case LastMonthNameLong:
        return QStringLiteral("%LASTMONTHNAMELONG");
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
    return date.toString(QStringLiteral("MMM"));
}

QString MessageComposer::ConvertSnippetVariablesUtil::monthNameLong()
{
    const QDate date = QDate::currentDate();
    return date.toString(QStringLiteral("MMMM"));
}

QString MessageComposer::ConvertSnippetVariablesUtil::lastMonthNameLong()
{
    const QDate date = QDate::currentDate().addMonths(-1);
    return date.toString(QStringLiteral("MMMM"));
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameShort()
{
    const QDate date = QDate::currentDate();
    return date.toString(QStringLiteral("ddd"));
}

QString MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong()
{
    const QDate date = QDate::currentDate();
    return date.toString(QStringLiteral("dddd"));
}

QString MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth()
{
    const QDate date = QDate::currentDate();
    return date.toString(date.toString(QStringLiteral("yyyy-MMM")));
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
