/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertvariablesjobtest.h"
#include "composer/composerviewbase.h"
#include "composer/composerviewinterface.h"
#include "snippet/convertsnippetvariablesjob.h"
#include <QTest>
QTEST_GUILESS_MAIN(ConvertVariablesJobTest)

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
    job.setText(QStringLiteral("bla"));
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
    b.setSubject(QStringLiteral("Subject!!!!"));
    b.setFrom(QStringLiteral("from!!"));
    // TODO add CC/BCC/TO

    job.setText(original);
    QCOMPARE(job.convertVariables(interface, original), expected);
}

void ConvertVariablesJobTest::shouldConvertVariables_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("novariable") << QStringLiteral("bla bli blo") << QStringLiteral("bla bli blo");
    QTest::newRow("subject") << QStringLiteral("bla bli blo %FULLSUBJECT") << QStringLiteral("bla bli blo Subject!!!!");
    // TODO add more autotests !
}

#include "moc_convertvariablesjobtest.cpp"
