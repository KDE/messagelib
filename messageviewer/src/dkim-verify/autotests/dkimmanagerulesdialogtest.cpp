/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulesdialogtest.h"
using namespace Qt::Literals::StringLiterals;

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

    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);

    auto mRulesWidget = w.findChild<MessageViewer::DKIMManageRulesWidget *>(u"mRulesWidget"_s);
    QVERIFY(mRulesWidget);

    auto buttonBox = w.findChild<QDialogButtonBox *>(u"buttonBox"_s);
    QVERIFY(buttonBox);
    QCOMPARE(buttonBox->standardButtons(), QDialogButtonBox::StandardButtons{QDialogButtonBox::Ok | QDialogButtonBox::Cancel});

    auto addButton = w.findChild<QPushButton *>(u"addButton"_s);
    QVERIFY(addButton);
    QVERIFY(!addButton->text().isEmpty());

    auto importButton = w.findChild<QPushButton *>(u"importButton"_s);
    QVERIFY(importButton);
    QVERIFY(!importButton->text().isEmpty());

    auto mExportButton = w.findChild<QPushButton *>(u"mExportButton"_s);
    QVERIFY(mExportButton);
    QVERIFY(!mExportButton->text().isEmpty());
    QVERIFY(!mExportButton->isEnabled());
}

#include "moc_dkimmanagerulesdialogtest.cpp"
