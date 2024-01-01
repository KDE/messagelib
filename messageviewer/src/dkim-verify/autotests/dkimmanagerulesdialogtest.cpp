/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulesdialogtest.h"
#include "dkim-verify/dkimmanagerulesdialog.h"
#include "dkim-verify/dkimmanageruleswidget.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(DKIMManageRulesDialogTest)

DKIMManageRulesDialogTest::DKIMManageRulesDialogTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
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
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Ok | QDialogButtonBox::Cancel});

    auto addButton = w.findChild<QPushButton *>(QStringLiteral("addButton"));
    QVERIFY(addButton);
    QVERIFY(!addButton->text().isEmpty());

    auto importButton = w.findChild<QPushButton *>(QStringLiteral("importButton"));
    QVERIFY(importButton);
    QVERIFY(!importButton->text().isEmpty());

    auto mExportButton = w.findChild<QPushButton *>(QStringLiteral("mExportButton"));
    QVERIFY(mExportButton);
    QVERIFY(!mExportButton->text().isEmpty());
    QVERIFY(!mExportButton->isEnabled());
}

#include "moc_dkimmanagerulesdialogtest.cpp"
