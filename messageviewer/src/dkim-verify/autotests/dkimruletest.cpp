/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimruletest.h"
#include "dkim-verify/dkimrule.h"
#include <QTest>
QTEST_GUILESS_MAIN(DKIMRuleTest)
DKIMRuleTest::DKIMRuleTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMRuleTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMRule rule;
    QVERIFY(rule.domain().isEmpty());
    QVERIFY(rule.signedDomainIdentifier().isEmpty());
    QVERIFY(rule.from().isEmpty());
    QVERIFY(rule.listId().isEmpty());
    QVERIFY(rule.enabled());
    QCOMPARE(rule.ruleType(), MessageViewer::DKIMRule::RuleType::Unknown);
    QCOMPARE(rule.priority(), 1000);
}

#include "moc_dkimruletest.cpp"
