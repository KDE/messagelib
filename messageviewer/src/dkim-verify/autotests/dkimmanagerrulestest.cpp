/*
   SPDX-FileCopyrightText: 2019-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerrulestest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimmanagerrules.h"
#include <QStandardPaths>
#include <QTest>
QTEST_GUILESS_MAIN(DKIMManagerRulesTest)
DKIMManagerRulesTest::DKIMManagerRulesTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void DKIMManagerRulesTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMManagerRules r;
    QVERIFY(r.rules().isEmpty());
    QVERIFY(r.isEmpty());
}

void DKIMManagerRulesTest::shouldAddRules()
{
    MessageViewer::DKIMManagerRules r;
    QVERIFY(r.isEmpty());
    MessageViewer::DKIMRule rule;
    rule.setDomain(u"bla"_s);
    rule.setFrom(u"foo"_s);
    rule.setRuleType(MessageViewer::DKIMRule::RuleType::MustBeSigned);
    r.addRule(rule);
    QVERIFY(!r.isEmpty());
    r.clear();
}

void DKIMManagerRulesTest::shouldClearRules()
{
    MessageViewer::DKIMManagerRules r;
    QVERIFY(r.isEmpty());
    MessageViewer::DKIMRule rule;
    rule.setDomain(u"bla"_s);
    rule.setFrom(u"foo"_s);
    rule.setRuleType(MessageViewer::DKIMRule::RuleType::MustBeSigned);
    r.addRule(rule);
    QVERIFY(!r.isEmpty());
    r.clear();
    QVERIFY(r.isEmpty());
}

// TODO add save/load support

#include "moc_dkimmanagerrulestest.cpp"
