/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkey.h"
#include "dkimutil.h"
#include <KConfig>
#include <KConfigGroup>
#include <QRegularExpression>
#include <QtCrypto>
using namespace MessageViewer;
DKIMManagerKey::DKIMManagerKey(QObject *parent)
    : QObject(parent)
    , mQcaInitializer(new QCA::Initializer(QCA::Practical, 64))
{
    loadKeys();
}

DKIMManagerKey::~DKIMManagerKey()
{
    delete mQcaInitializer;
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
    const QStringList keyGroups = config->groupList().filter(QRegularExpression(QStringLiteral("DKIM Key Record #\\d+")));
    return keyGroups;
}

void DKIMManagerKey::loadKeys()
{
    KSharedConfig::Ptr config;
    const QStringList keyGroups = keyRecorderList(config);

    mKeys.clear();
    for (const QString &groupName : keyGroups) {
        KConfigGroup group = config->group(groupName);
        const QString selector = group.readEntry(QStringLiteral("Selector"), QString());
        const QString domain = group.readEntry(QStringLiteral("Domain"), QString());
        const QString key = group.readEntry(QStringLiteral("Key"), QString());
        const QDateTime storedAt = QDateTime::fromString(group.readEntry(QStringLiteral("StoredAt"), QString()), Qt::ISODate);
        QDateTime lastUsed = QDateTime::fromString(group.readEntry(QStringLiteral("LastUsed"), QString()), Qt::ISODate);
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
        const QString groupName = QStringLiteral("DKIM Key Record #%1").arg(i);
        KConfigGroup group = config->group(groupName);
        const KeyInfo &info = mKeys.at(i);
        group.writeEntry(QStringLiteral("Selector"), info.selector);
        group.writeEntry(QStringLiteral("Domain"), info.domain);
        group.writeEntry(QStringLiteral("Key"), info.keyValue);
        group.writeEntry(QStringLiteral("StoredAt"), info.storedAtDateTime.toString(Qt::ISODate));
        group.writeEntry(QStringLiteral("LastUsed"), info.lastUsedDateTime.toString(Qt::ISODate));
    }
}

void DKIMManagerKey::saveKeys(const QList<MessageViewer::KeyInfo> &lst)
{
    mKeys = lst;
    saveKeys();
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
