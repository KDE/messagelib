/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "convertvariablesjobtest.h"
#include "snippet/convertvariablesjob.h"
#include <QTest>
#include "composer/composerviewbase.h"
#include "composer/composerviewinterface.h"
QTEST_GUILESS_MAIN(ConvertVariablesJobTest)

ConvertVariablesJobTest::ConvertVariablesJobTest(QObject *parent)
    : QObject(parent)
{

}

void ConvertVariablesJobTest::shouldHaveDefaultValues()
{
    MessageComposer::ConvertVariablesJob job;
    QVERIFY(job.text().isEmpty());
    QVERIFY(!job.composerViewInterface());
    QVERIFY(!job.canStart());
}

void ConvertVariablesJobTest::shouldCanStart()
{
    MessageComposer::ConvertVariablesJob job;
    QVERIFY(!job.canStart());
    job.setText(QStringLiteral("bla"));
    QVERIFY(!job.canStart());
    MessageComposer::ComposerViewBase b;
    MessageComposer::ComposerViewInterface *interface = new MessageComposer::ComposerViewInterface(&b);
    job.setComposerViewInterface(interface);
    QVERIFY(job.canStart());
}

void ConvertVariablesJobTest::shouldConvertVariables()
{
    QFETCH(QString, original);
    QFETCH(QString, expected);
    MessageComposer::ComposerViewBase b;
    MessageComposer::ComposerViewInterface *interface = new MessageComposer::ComposerViewInterface(&b);
    MessageComposer::ConvertVariablesJob job;
    job.setComposerViewInterface(interface);
    b.setSubject(QStringLiteral("Subject!!!!"));
    b.setFrom(QStringLiteral("from!!"));
    //TOOD add CC/BCC/TO

    job.setText(original);
    QCOMPARE(job.convertVariables(), expected);
}

void ConvertVariablesJobTest::shouldConvertVariables_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("expected");
    QTest::newRow("empty") << QString() << QString();
    QTest::newRow("novariable") << QStringLiteral("bla bli blo") << QStringLiteral("bla bli blo");
    QTest::newRow("subject") << QStringLiteral("bla bli blo %FULLSUBJECT") << QStringLiteral("bla bli blo Subject!!!!");
}
