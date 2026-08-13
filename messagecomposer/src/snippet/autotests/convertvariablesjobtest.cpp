/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertvariablesjobtest.h"

#include "composer/composerviewbase.h"
#include "composer/composerviewinterface.h"
#include "snippet/convertsnippetvariablesjob.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <QTest>
QTEST_GUILESS_MAIN(ConvertVariablesJobTest)

using namespace Qt::Literals::StringLiterals;
ConvertVariablesJobTest::ConvertVariablesJobTest(QObject *parent)
    : QObject(parent)
{
}

void ConvertVariablesJobTest::shouldHaveDefaultValues()
{
    MessageComposer::ConvertSnippetVariablesJob job;
    QVERIFY(job.text().isEmpty());
    QVERIFY(!job.composerViewInterface());
    QVERIFY(!job.canStart());
}

void ConvertVariablesJobTest::shouldCanStart()
{
    MessageComposer::ConvertSnippetVariablesJob job;
    QVERIFY(!job.canStart());
    job.setText(u"bla"_s);
    QVERIFY(!job.canStart());
    MessageComposer::ComposerViewBase b;
    auto interface = new MessageComposer::ComposerViewInterface(&b);
    job.setComposerViewInterface(interface);
    QVERIFY(job.canStart());
}

void ConvertVariablesJobTest::shouldConvertVariables()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    MessageComposer::ComposerViewBase b;
    auto interface = new MessageComposer::ComposerViewInterface(&b);
    MessageComposer::ConvertSnippetVariablesJob job;
    job.setComposerViewInterface(interface);
    b.setSubject(u"Subject!!!!"_s);
    b.setFrom(u"from!!"_s);
    // TODO add CC/BCC/TO

    job.setText(original);
    QCOMPARE(job.convertVariables(interface, original), expected);
}

void ConvertVariablesJobTest::shouldConvertVariables_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("novariable") << u"bla bli blo"_s << QStringLiteral("bla bli blo");
    QTest::newRow("subject") << u"bla bli blo %FULLSUBJECT"_s << QStringLiteral("bla bli blo Subject!!!!");
    QTest::newRow("subjecttwice") << u"%FULLSUBJECT %FULLSUBJECT"_s << QStringLiteral("Subject!!!! Subject!!!!");
    QTest::newRow("subjectfollowedbytext") << u"%FULLSUBJECTbla"_s << QStringLiteral("Subject!!!!bla");
    QTest::newRow("unknownvariable") << u"%UNKNOWNVARIABLE"_s << QStringLiteral("%UNKNOWNVARIABLE");
    QTest::newRow("percentbeforevariable") << u"%%FULLSUBJECT"_s << QStringLiteral("%Subject!!!!");
    // Variables are matched by prefix: make sure the longest name always wins.
    QTest::newRow("year") << u"%YEAR"_s << MessageComposer::ConvertSnippetVariablesUtil::year();
    QTest::newRow("yearlastmonth") << u"%YEARLASTMONTH"_s << MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth();
    QTest::newRow("yearandyearlastmonth") << u"%YEAR %YEARLASTMONTH"_s
                                          << (MessageComposer::ConvertSnippetVariablesUtil::year() + u' '
                                              + MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth());
    QTest::newRow("monthnamelong") << u"%MONTHNAMELONG"_s << MessageComposer::ConvertSnippetVariablesUtil::monthNameLong();
    QTest::newRow("lastmonthnamelong") << u"%LASTMONTHNAMELONG"_s << MessageComposer::ConvertSnippetVariablesUtil::lastMonthNameLong();
    QTest::newRow("dayofweeknamelong") << u"%DAYOFWEEKNAMELONG"_s << MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong();
    // TODO add CC/BCC/TO autotests !
}

#include "moc_convertvariablesjobtest.cpp"
