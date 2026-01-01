/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerrules.h"
using namespace Qt::Literals::StringLiterals;

#include "dkimutil.h"
#include <KConfigGroup>
#include <QRegularExpression>

using namespace MessageViewer;
DKIMManagerRules::DKIMManagerRules(QObject *parent)
    : QObject(parent)
{
    (void)loadRules();
}

DKIMManagerRules::~DKIMManagerRules()
{
    save();
}

DKIMManagerRules *DKIMManagerRules::self()
{
    static DKIMManagerRules s_self;
    return &s_self;
}

void DKIMManagerRules::addRule(const DKIMRule &rule)
{
    if (!mRules.contains(rule)) {
        mRules.append(rule);
        save();
    }
}

bool DKIMManagerRules::isEmpty() const
{
    return mRules.isEmpty();
}

QList<DKIMRule> DKIMManagerRules::rules() const
{
    return mRules;
}

QStringList DKIMManagerRules::ruleGroups(const KSharedConfig::Ptr &config) const
{
    return config->groupList().filter(QRegularExpression(u"DKIM Rule #\\d+"_s));
}

int DKIMManagerRules::loadRules(const QString &fileName)
{
    const KSharedConfig::Ptr &config =
        KSharedConfig::openConfig(fileName.isEmpty() ? MessageViewer::DKIMUtil::defaultConfigFileName() : fileName, KConfig::NoGlobals);
    const QStringList rulesGroups = ruleGroups(config);

    if (fileName.isEmpty()) {
        mRules.clear();
    }
    int numberOfRulesAdded = 0;
    for (const QString &groupName : rulesGroups) {
        KConfigGroup group = config->group(groupName);
        const QStringList signedDomainIdentifier = group.readEntry(u"SignedDomainIdentifier"_s, QStringList());
        const QString from = group.readEntry(u"From"_s, QString());
        const QString domain = group.readEntry(u"Domain"_s, QString());
        const bool enabled = group.readEntry(u"Enabled"_s, true);
        const int ruleType = group.readEntry(u"RuleType"_s, 0);
        const QString listId = group.readEntry(u"List-Id"_s, QString());
        const int priority = group.readEntry(u"Priority"_s, 1000);
        DKIMRule rule;
        rule.setEnabled(enabled);
        rule.setDomain(domain);
        rule.setFrom(from);
        rule.setListId(listId);
        rule.setSignedDomainIdentifier(signedDomainIdentifier);
        rule.setRuleType(static_cast<DKIMRule::RuleType>(ruleType));
        rule.setPriority(priority);
        if (rule.isValid()) {
            numberOfRulesAdded++;
            mRules.append(rule);
        }
    }
    return numberOfRulesAdded;
}

void DKIMManagerRules::saveRules(const QList<DKIMRule> &lst)
{
    mRules = lst;
    save();
}

void DKIMManagerRules::clear()
{
    mRules.clear();
    save();
}

int DKIMManagerRules::importRules(const QString &fileName)
{
    return loadRules(fileName);
}

void DKIMManagerRules::exportRules(const QString &fileName, const QList<DKIMRule> &lst)
{
    save(fileName, lst);
}

void DKIMManagerRules::save(const QString &fileName, const QList<DKIMRule> &lst)
{
    const KSharedConfig::Ptr &config =
        KSharedConfig::openConfig(fileName.isEmpty() ? MessageViewer::DKIMUtil::defaultConfigFileName() : fileName, KConfig::NoGlobals);
    const QStringList rulesGroups = ruleGroups(config);

    for (const QString &group : rulesGroups) {
        config->deleteGroup(group);
    }
    QList<DKIMRule> rules = lst;
    if (lst.isEmpty()) {
        rules = mRules;
    }
    for (int i = 0, total = rules.count(); i < total; ++i) {
        const QString groupName = u"DKIM Rule #%1"_s.arg(i);
        KConfigGroup group = config->group(groupName);
        const DKIMRule &rule = rules.at(i);

        group.writeEntry(u"SignedDomainIdentifier"_s, rule.signedDomainIdentifier());
        group.writeEntry(u"From"_s, rule.from());
        group.writeEntry(u"Domain"_s, rule.domain());
        group.writeEntry(u"Enabled"_s, rule.enabled());
        group.writeEntry(u"RuleType"_s, static_cast<int>(rule.ruleType()));
        group.writeEntry(u"List-Id"_s, rule.listId());
        group.writeEntry(u"Priority"_s, rule.priority());
    }
}

#include "moc_dkimmanagerrules.cpp"
