/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulescomboboxtest.h"
#include "dkim-verify/dkimmanagerulescombobox.h"
#include <QTest>
QTEST_MAIN(DKIMManageRulesComboBoxTest)

DKIMManageRulesComboBoxTest::DKIMManageRulesComboBoxTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMManageRulesComboBoxTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMManageRulesComboBox w;
    QVERIFY(w.count() > 0);
}

void DKIMManageRulesComboBoxTest::shouldChangeIndex()
{
    MessageViewer::DKIMManageRulesComboBox w;
    w.setCurrentIndex(1);
    QCOMPARE(w.ruleType(), MessageViewer::DKIMRule::RuleType::CanBeSigned);
}

#include "moc_dkimmanagerulescomboboxtest.cpp"
