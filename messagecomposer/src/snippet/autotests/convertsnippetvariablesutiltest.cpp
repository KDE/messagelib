/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesutiltest.h"
using namespace Qt::Literals::StringLiterals;

#include "snippet/convertsnippetvariablesutil.h"
#include <QTest>

QTEST_GUILESS_MAIN(ConvertSnippetVariablesUtilTest)

ConvertSnippetVariablesUtilTest::ConvertSnippetVariablesUtilTest(QObject *parent)
    : QObject(parent)
{
}

void ConvertSnippetVariablesUtilTest::shouldTranslateVariables()
{
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcAddr), u"%CCADDR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcLname), u"%CCLNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcFname), u"%CCFNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CcName), u"%CCNAME"_s);

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccAddr), u"%BCCADDR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccLname), u"%BCCLNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccFname), u"%BCCFNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::BccName), u"%BCCNAME"_s);

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FullSubject),
             u"%FULLSUBJECT"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToAddr), u"%TOADDR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToFname), u"%TOFNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToLname), u"%TOLNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ToName), u"%TONAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromAddr), u"%FROMADDR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromLname), u"%FROMLNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromFname), u"%FROMFNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::FromName), u"%FROMNAME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Dow), u"%DOW"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentName),
             u"%ATTACHMENTNAMES"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Date), u"%DATE"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::ShortDate), u"%SHORTDATE"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Time), u"%TIME"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::TimeLong), u"%TIMELONG"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentCount),
             u"%ATTACHMENTCOUNT"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentFilenames),
             u"%ATTACHMENTFILENAMES"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::AttachmentNamesAndSizes),
             u"%ATTACHMENTNAMESANDSIZES"_s);

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::Year), u"%YEAR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::LastYear), u"%LASTYEAR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::NextYear), u"%NEXTYEAR"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNumber),
             u"%MONTHNUMBER"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayNumber), u"%DAYNUMBER"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfMonth), u"%DAYOFMONTH"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::CustomDate), u"%CUSTOMDATE"_s);

    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::WeekNumber), u"%WEEKNUMBER"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNameShort),
             u"%MONTHNAMESHORT"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::MonthNameLong),
             u"%MONTHNAMELONG"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeek), u"%DAYOFWEEK"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameShort),
             u"%DAYOFWEEKNAMESHORT"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameLong),
             u"%DAYOFWEEKNAMELONG"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::YearLastMonth),
             u"%YEARLASTMONTH"_s);
    QCOMPARE(MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::LastMonthNameLong),
             u"%LASTMONTHNAMELONG"_s);
}

#include "moc_convertsnippetvariablesutiltest.cpp"
