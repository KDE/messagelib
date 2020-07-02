/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimruledialogtest.h"
#include "dkim-verify/dkimruledialog.h"
#include "dkim-verify/dkimrulewidget.h"
#include <QDialogButtonBox>
#include <QTest>
#include <QVBoxLayout>

QTEST_MAIN(DKIMRuleDialogTest)

DKIMRuleDialogTest::DKIMRuleDialogTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMRuleDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMRuleDialog dlg;
    QVERIFY(!dlg.windowTitle().isEmpty());

    QVBoxLayout *mainLayout = dlg.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);

    MessageViewer::DKIMRuleWidget *rulewidget = dlg.findChild<MessageViewer::DKIMRuleWidget *>(QStringLiteral("rulewidget"));
    QVERIFY(rulewidget);

    QDialogButtonBox *buttonBox = dlg.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), {QDialogButtonBox::Ok | QDialogButtonBox::Cancel});
}
