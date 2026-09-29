/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"
#include <KSharedConfig>
#include <QDateTime>
#include <QList>
#include <QObject>

namespace MessageCore
{
/*!
 * \struct MessageCore::KeyInfo
 * \inmodule MessageCore
 * \inheaderfile MessageCore/KeyInfo
 * \brief The KeyInfo struct
 * \author Laurent Montel <montel@kde.org>
 */
struct MESSAGECORE_EXPORT KeyInfo {
    KeyInfo() = default;

    KeyInfo(const QString &key, const QString &sel, const QString &dom, const QDateTime &storedAt = {}, const QDateTime &lastUsed = {})
        : keyValue(key)
        , selector(sel)
        , domain(dom)
        , storedAtDateTime(storedAt)
        , lastUsedDateTime(lastUsed)
    {
    }

    QString keyValue;
    QString selector;
    QString domain;
    QDateTime storedAtDateTime;
    QDateTime lastUsedDateTime;
    [[nodiscard]] bool operator==(const KeyInfo &) const;
    [[nodiscard]] bool operator!=(const KeyInfo &) const;
};

/*!
 * \class MessageCore::DKIMManagerKey
 * \inmodule MessageCore
 * \inheaderfile MessageCore/DKIMManagerKey
 * \brief The DKIMManagerKey class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT DKIMManagerKey : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    ~DKIMManagerKey() override;

    /*!
     */
    static DKIMManagerKey *self();

    /*!
     */
    void loadKeys();
    /*!
     */
    void saveKeys(const QList<KeyInfo> &lst);

    /*!
     */
    void addKey(const KeyInfo &key);

    /*!
     */
    [[nodiscard]] QList<KeyInfo> keys() const;

    /*!
     */
    void saveKeys();
    /*!
     */
    [[nodiscard]] QString keyValue(const QString &selector, const QString &domain);

    /*!
     */
    void updateLastUsed(const QString &selector, const QString &domain);

private:
    explicit DKIMManagerKey(QObject *parent = nullptr);
    [[nodiscard]] MESSAGECORE_NO_EXPORT QStringList keyRecorderList(KSharedConfig::Ptr &config) const;
    QList<KeyInfo> mKeys;
};
}
Q_DECLARE_TYPEINFO(MessageCore::KeyInfo, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(MessageCore::KeyInfo)
MESSAGECORE_EXPORT QDebug operator<<(QDebug d, const MessageCore::KeyInfo &t);
