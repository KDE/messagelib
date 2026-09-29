/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerrulestest.h"

#include "dkim-verify/dkimmanagerrules.h"
#include <QStandardPaths>
#include <QTest>
using namespace Qt::Literals::StringLiterals;
QTEST_GUILESS_MAIN(DKIMManagerRulesTest)
DKIMManagerRulesTest::DKIMManagerRulesTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMManagerRulesTest::shouldHaveDefaultValues()
{
    MessageCore::DKIMManagerRules r;
    QVERIFY(r.rules().isEmpty());
    QVERIFY(r.isEmpty());
}

void DKIMManagerRulesTest::shouldAddRules()
{
    MessageCore::DKIMManagerRules r;
    QVERIFY(r.isEmpty());
    MessageCore::DKIMRule rule;
    rule.setDomain(u"bla"_s);
    rule.setFrom(u"foo"_s);
    rule.setRuleType(MessageCore::DKIMRule::RuleType::MustBeSigned);
    r.addRule(rule);
    QVERIFY(!r.isEmpty());
    r.clear();
}

void DKIMManagerRulesTest::shouldClearRules()
{
    MessageCore::DKIMManagerRules r;
    QVERIFY(r.isEmpty());
    MessageCore::DKIMRule rule;
    rule.setDomain(u"bla"_s);
    rule.setFrom(u"foo"_s);
    rule.setRuleType(MessageCore::DKIMRule::RuleType::MustBeSigned);
    r.addRule(rule);
    QVERIFY(!r.isEmpty());
    r.clear();
    QVERIFY(r.isEmpty());
}

// TODO add save/load support

#include "moc_dkimmanagerrulestest.cpp"
