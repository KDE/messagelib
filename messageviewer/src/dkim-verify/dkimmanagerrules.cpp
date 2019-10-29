/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include <KConfigGroup>
#include <KSharedConfig>
#include <QRegularExpression>
using namespace MessageViewer;
DKIMManagerRules::DKIMManagerRules(QObject *parent)
    : QObject(parent)
{

}

DKIMManagerRules::~DKIMManagerRules()
{

}

DKIMManagerRules *DKIMManagerRules::self()
{
    static DKIMManagerRules s_self;
    return &s_self;
}

QVector<DKIMRule> DKIMManagerRules::rules() const
{
    return mRules;
}

void DKIMManagerRules::loadRules()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig();
    const QStringList keyGroups
        = config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Rule #\\d+")));

    mRules.clear();
    for (const QString &groupName : keyGroups) {
        KConfigGroup group = config->group(groupName);
        const QStringList signedDomainIdentifier = group.readEntry(QLatin1String("SignedDomainIdentifier"), QStringList());
        const QString from = group.readEntry(QLatin1String("From"), QString());
        const QString domain = group.readEntry(QLatin1String("Domain"), QString());
        const bool enabled = group.readEntry(QLatin1String("Enabled"), true);
        DKIMRule rule;
        rule.setEnabled(enabled);
        rule.setDomain(domain);
        rule.setFrom(from);
        rule.setSignedDomainIdentifier(signedDomainIdentifier);
        mRules.append(rule);
    }
}

void DKIMManagerRules::saveRules(const QVector<DKIMRule> &lst)
{
    mRules = lst;
    save();
}

void DKIMManagerRules::save()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig();
    const QStringList filterGroups
        = config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Rule #\\d+")));

    for (const QString &group : filterGroups) {
        config->deleteGroup(group);
    }
    for (int i = 0, total = mRules.count(); i < total; ++i) {
        const QString groupName = QStringLiteral("DKIM Key Record #%1").arg(i);
        KConfigGroup group = config->group(groupName);
        const DKIMRule &rule = mRules.at(i);

        group.writeEntry(QLatin1String("SignedDomainIdentifier"), rule.signedDomainIdentifier());
        group.writeEntry(QLatin1String("From"), rule.from());
        group.writeEntry(QLatin1String("Domain"), rule.domain());
        group.writeEntry(QLatin1String("Enabled"), rule.enabled());
    }

}
