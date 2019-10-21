/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkimmanagerkey.h"

#include <KSharedConfig>
#include <KConfig>
#include <QRegularExpression>
#include <KConfigGroup>

using namespace MessageViewer;
DKIMManagerKey::DKIMManagerKey(QObject *parent)
    : QObject(parent)
{
    loadKeys();
}

DKIMManagerKey::~DKIMManagerKey()
{
}

DKIMManagerKey *DKIMManagerKey::self()
{
    static DKIMManagerKey s_self;
    return &s_self;
}

QString DKIMManagerKey::keyValue(const QString &selector, const QString &domain)
{
    for (const KeyInfo &keyInfo : qAsConst(mKeys)) {
        if (keyInfo.selector == selector && keyInfo.domain == domain) {
            return keyInfo.keyValue;
        }
    }
    return {};
}

void DKIMManagerKey::addKey(const KeyInfo &key)
{
    if (!mKeys.contains(key)) {
        mKeys.append(key);
    }
}

void DKIMManagerKey::removeKey(const QString &key)
{
    for (const KeyInfo &keyInfo : qAsConst(mKeys)) {
        if (keyInfo.keyValue == key) {
            mKeys.removeAll(keyInfo);
            break;
        }
    }
}

QVector<KeyInfo> DKIMManagerKey::keys() const
{
    return mKeys;
}

void DKIMManagerKey::loadKeys()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig();
    const QStringList keyGroups
        = config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Key Record #\\d+")));

    mKeys.clear();
    for (const QString &groupName : keyGroups) {
        KConfigGroup group = config->group(groupName);
        const QString selector = group.readEntry(QLatin1String("Selector"), QString());
        const QString domain = group.readEntry(QLatin1String("Domain"), QString());
        const QString key = group.readEntry(QLatin1String("Key"), QString());
        mKeys.append(KeyInfo{key, selector, domain});
    }
}

void DKIMManagerKey::saveKeys()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig();
    const QStringList filterGroups
        = config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Key Record #\\d+")));

    for (const QString &group : filterGroups) {
        config->deleteGroup(group);
    }
    for (int i = 0, total = mKeys.count(); i < total; ++i) {
        const QString groupName = QStringLiteral("DKIM Key Record #%1").arg(i);
        KConfigGroup group = config->group(groupName);
        const KeyInfo &info = mKeys.at(i);
        group.writeEntry(QLatin1String("Selector"), info.selector);
        group.writeEntry(QLatin1String("Domain"), info.domain);
        group.writeEntry(QLatin1String("Key"), info.keyValue);
    }
}

void DKIMManagerKey::saveKeys(const QVector<MessageViewer::KeyInfo> &lst)
{
    mKeys = lst;
    saveKeys();
}

bool KeyInfo::operator ==(const KeyInfo &other) const
{
    return keyValue == other.keyValue
           && selector == other.selector
           && domain == other.domain;
}
