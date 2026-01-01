/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkey.h"
using namespace Qt::Literals::StringLiterals;

#include "dkimutil.h"
#include <KConfig>
#include <KConfigGroup>
#include <QRegularExpression>

using namespace MessageViewer;
DKIMManagerKey::DKIMManagerKey(QObject *parent)
    : QObject(parent)
{
    loadKeys();
}

DKIMManagerKey::~DKIMManagerKey()
{
    saveKeys();
}

DKIMManagerKey *DKIMManagerKey::self()
{
    static DKIMManagerKey s_self;
    return &s_self;
}

QString DKIMManagerKey::keyValue(const QString &selector, const QString &domain)
{
    for (const KeyInfo &keyInfo : std::as_const(mKeys)) {
        if (keyInfo.selector == selector && keyInfo.domain == domain) {
            return keyInfo.keyValue;
        }
    }
    return {};
}

void DKIMManagerKey::updateLastUsed(const QString &selector, const QString &domain)
{
    for (const KeyInfo &keyInfo : std::as_const(mKeys)) {
        if (keyInfo.selector == selector && keyInfo.domain == domain) {
            KeyInfo newKey = keyInfo;
            newKey.lastUsedDateTime = QDateTime::currentDateTime();
            addKey(newKey);
            return;
        }
    }
}

void DKIMManagerKey::addKey(const KeyInfo &key)
{
    const QList<KeyInfo> keys = mKeys;
    for (const KeyInfo &keyInfo : keys) {
        if (keyInfo.selector == key.selector && keyInfo.domain == key.domain) {
            mKeys.removeAll(keyInfo);
        }
    }
    mKeys.append(key);
}

void DKIMManagerKey::removeKey(const QString &key)
{
    for (const KeyInfo &keyInfo : std::as_const(mKeys)) {
        if (keyInfo.keyValue == key) {
            mKeys.removeAll(keyInfo);
            break;
        }
    }
}

QList<KeyInfo> DKIMManagerKey::keys() const
{
    return mKeys;
}

QStringList DKIMManagerKey::keyRecorderList(KSharedConfig::Ptr &config) const
{
    config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    const QStringList keyGroups = config->groupList().filter(QRegularExpression(u"DKIM Key Record #\\d+"_s));
    return keyGroups;
}

void DKIMManagerKey::loadKeys()
{
    KSharedConfig::Ptr config;
    const QStringList keyGroups = keyRecorderList(config);

    mKeys.clear();
    for (const QString &groupName : keyGroups) {
        KConfigGroup group = config->group(groupName);
        const QString selector = group.readEntry(u"Selector"_s, QString());
        const QString domain = group.readEntry(u"Domain"_s, QString());
        const QString key = group.readEntry(u"Key"_s, QString());
        const QDateTime storedAt = QDateTime::fromString(group.readEntry(u"StoredAt"_s, QString()), Qt::ISODate);
        QDateTime lastUsed = QDateTime::fromString(group.readEntry(u"LastUsed"_s, QString()), Qt::ISODate);
        if (!lastUsed.isValid()) {
            lastUsed = storedAt;
        }
        mKeys.append(KeyInfo{key, selector, domain, storedAt, lastUsed});
    }
}

void DKIMManagerKey::saveKeys()
{
    KSharedConfig::Ptr config;
    const QStringList filterGroups = keyRecorderList(config);

    for (const QString &group : filterGroups) {
        config->deleteGroup(group);
    }
    for (int i = 0, total = mKeys.count(); i < total; ++i) {
        const QString groupName = u"DKIM Key Record #%1"_s.arg(i);
        KConfigGroup group = config->group(groupName);
        const KeyInfo &info = mKeys.at(i);
        group.writeEntry(u"Selector"_s, info.selector);
        group.writeEntry(u"Domain"_s, info.domain);
        group.writeEntry(u"Key"_s, info.keyValue);
        group.writeEntry(u"StoredAt"_s, info.storedAtDateTime.toString(Qt::ISODate));
        group.writeEntry(u"LastUsed"_s, info.lastUsedDateTime.toString(Qt::ISODate));
    }
}

void DKIMManagerKey::saveKeys(const QList<MessageViewer::KeyInfo> &lst)
{
    if (mKeys != lst) {
        mKeys = lst;
        saveKeys();
    }
}

bool KeyInfo::operator==(const KeyInfo &other) const
{
    return keyValue == other.keyValue && selector == other.selector && domain == other.domain;
}

bool KeyInfo::operator!=(const KeyInfo &other) const
{
    return !(operator==(other));
}

QDebug operator<<(QDebug d, const KeyInfo &t)
{
    d << " keyvalue " << t.keyValue;
    d << " selector " << t.selector;
    d << " domain " << t.domain;
    d << " storedAtDateTime " << t.storedAtDateTime;
    d << " lastUsedDateTime " << t.lastUsedDateTime;
    return d;
}

#include "moc_dkimmanagerkey.cpp"
