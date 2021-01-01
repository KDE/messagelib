/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulesdialogtest.h"
#include "dkim-verify/dkimmanagerulesdialog.h"
#include "dkim-verify/dkimmanageruleswidget.h"
#include <QDialogButtonBox>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(DKIMManageRulesDialogTest)

DKIMManageRulesDialogTest::DKIMManageRulesDialogTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMManageRulesDialogTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMManageRulesDialog w;
    QVERIFY(!w.windowTitle().isEmpty());

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);

    auto mRulesWidget = w.findChild<MessageViewer::DKIMManageRulesWidget *>(QStringLiteral("mRulesWidget"));
    QVERIFY(mRulesWidget);

    auto buttonBox = w.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), {QDialogButtonBox::Ok | QDialogButtonBox::Cancel});
}
