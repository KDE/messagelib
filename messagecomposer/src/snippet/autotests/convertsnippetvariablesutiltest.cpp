/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesutiltest.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <QTest>

QTEST_GUILESS_MAIN(ConvertSnippetVariablesUtilTest)

ConvertSnippetVariablesUtilTest::ConvertSnippetVariablesUtilTest(QObject *parent)
    : QObject(parent)
{
}

void ConvertSnippetVariablesUtilTest::shouldTranslateVariables()
{
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcAddr),
             QStringLiteral("%CCADDR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcLname),
             QStringLiteral("%CCLNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcFname),
             QStringLiteral("%CCFNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcName),
             QStringLiteral("%CCNAME"));

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccAddr),
             QStringLiteral("%BCCADDR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccLname),
             QStringLiteral("%BCCLNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccFname),
             QStringLiteral("%BCCFNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccName),
             QStringLiteral("%BCCNAME"));

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FullSubject),
             QStringLiteral("%FULLSUBJECT"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToAddr),
             QStringLiteral("%TOADDR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToFname),
             QStringLiteral("%TOFNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToLname),
             QStringLiteral("%TOLNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToName),
             QStringLiteral("%TONAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromAddr),
             QStringLiteral("%FROMADDR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromLname),
             QStringLiteral("%FROMLNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromFname),
             QStringLiteral("%FROMFNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromName),
             QStringLiteral("%FROMNAME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Dow), QStringLiteral("%DOW"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentName),
             QStringLiteral("%ATTACHMENTNAMES"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Date),
             QStringLiteral("%DATE"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ShortDate),
             QStringLiteral("%SHORTDATE"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Time),
             QStringLiteral("%TIME"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::TimeLong),
             QStringLiteral("%TIMELONG"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentCount),
             QStringLiteral("%ATTACHMENTCOUNT"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentFilenames),
             QStringLiteral("%ATTACHMENTFILENAMES"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentNamesAndSizes),
             QStringLiteral("%ATTACHMENTNAMESANDSIZES"));

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Year),
             QStringLiteral("%YEAR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::LastYear),
             QStringLiteral("%LASTYEAR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::NextYear),
             QStringLiteral("%NEXTYEAR"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNumber),
             QStringLiteral("%MONTHNUMBER"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfMonth),
             QStringLiteral("%DAYOFMONTH"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::WeekNumber),
             QStringLiteral("%WEEKNUMBER"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNameShort),
             QStringLiteral("%MONTHNAMESHORT"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNameLong),
             QStringLiteral("%MONTHNAMELONG"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeek),
             QStringLiteral("%DAYOFWEEK"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameShort),
             QStringLiteral("%DAYOFWEEKNAMESHORT"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameLong),
             QStringLiteral("%DAYOFWEEKNAMELONG"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::YearLastMonth),
             QStringLiteral("%YEARLASTMONTH"));
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::LastMonthNameLong),
             QStringLiteral("%LASTMONTHNAMELONG"));
}

#include "moc_convertsnippetvariablesutiltest.cpp"
