/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimruledialogtest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimruledialog.h"
#include "dkim-verify/dkimrulewidget.h"
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>

QTEST_MAIN(DKIMRuleDialogTest)

DKIMRuleDialogTest::DKIMRuleDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMRuleDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMRuleDialog dlg;
    QVERIFY(!dlg.windowTitle().isEmpty());

    auto mainLayout = dlg.findChild<QVBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);

    auto rulewidget = dlg.findChild<MessageViewer::DKIMRuleWidget *>(u"rulewidget"_s);
    QVERIFY(rulewidget);

    auto buttonBox = dlg.findChild<QDialogButtonBox *>(u"buttonBox"_s);
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Ok | QDialogButtonBox::Cancel});
}

#include "moc_dkimruledialogtest.cpp"
