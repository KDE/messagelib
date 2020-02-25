/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimmanagerrules.h"
#include "dkimutil.h"
#include <KConfigGroup>
#include <KSharedConfig>
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

QVector<DKIMRule> DKIMManagerRules::rules() const
{
    return mRules;
}

QStringList DKIMManagerRules::ruleGroups(const KSharedConfig::Ptr &config) const
{
    return config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Rule #\\d+")));
}

int DKIMManagerRules::loadRules(const QString &fileName)
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(fileName.isEmpty() ? MessageViewer::DKIMUtil::defaultConfigFileName() : fileName, KConfig::NoGlobals);
    const QStringList rulesGroups = ruleGroups(config);

    if (fileName.isEmpty()) {
        mRules.clear();
    }
    int numberOfRulesAdded = 0;
    for (const QString &groupName : rulesGroups) {
        KConfigGroup group = config->group(groupName);
        const QStringList signedDomainIdentifier = group.readEntry(QStringLiteral("SignedDomainIdentifier"), QStringList());
        const QString from = group.readEntry(QStringLiteral("From"), QString());
        const QString domain = group.readEntry(QStringLiteral("Domain"), QString());
        const bool enabled = group.readEntry(QStringLiteral("Enabled"), true);
        const int ruleType = group.readEntry(QStringLiteral("RuleType"), 0);
        const QString listId = group.readEntry(QStringLiteral("List-Id"), QString());
        const int priority = group.readEntry(QStringLiteral("Priority"), 1000);
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

void DKIMManagerRules::saveRules(const QVector<DKIMRule> &lst)
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

void DKIMManagerRules::exportRules(const QString &fileName)
{
    save(fileName);
}

void DKIMManagerRules::save(const QString &fileName)
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(fileName.isEmpty() ? MessageViewer::DKIMUtil::defaultConfigFileName() : fileName, KConfig::NoGlobals);
    const QStringList rulesGroups = ruleGroups(config);

    for (const QString &group : rulesGroups) {
        config->deleteGroup(group);
    }
    for (int i = 0, total = mRules.count(); i < total; ++i) {
        const QString groupName = QStringLiteral("DKIM Rule #%1").arg(i);
        KConfigGroup group = config->group(groupName);
        const DKIMRule &rule = mRules.at(i);

        group.writeEntry(QStringLiteral("SignedDomainIdentifier"), rule.signedDomainIdentifier());
        group.writeEntry(QStringLiteral("From"), rule.from());
        group.writeEntry(QStringLiteral("Domain"), rule.domain());
        group.writeEntry(QStringLiteral("Enabled"), rule.enabled());
        group.writeEntry(QStringLiteral("RuleType"), static_cast<int>(rule.ruleType()));
        group.writeEntry(QStringLiteral("List-Id"), rule.listId());
        group.writeEntry(QStringLiteral("Priority"), rule.priority());
    }
}
