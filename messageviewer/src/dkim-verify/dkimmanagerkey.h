/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <KSharedConfig>
#include <QDateTime>
#include <QList>
#include <QObject>
namespace QCA
{
class Initializer;
}
namespace MessageViewer
{
/**
 * @brief The KeyInfo struct
 * @author Laurent Montel <montel@kde.org>
 */
struct MESSAGEVIEWER_EXPORT KeyInfo {
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

/**
 * @brief The DKIMManagerKey class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerKey : public QObject
{
    Q_OBJECT
public:
    ~DKIMManagerKey() override;

    static DKIMManagerKey *self();

    void loadKeys();
    void saveKeys(const QList<KeyInfo> &lst);

    void addKey(const KeyInfo &key);
    void removeKey(const QString &key);

    [[nodiscard]] QList<KeyInfo> keys() const;

    void saveKeys();
    [[nodiscard]] QString keyValue(const QString &selector, const QString &domain);

    void updateLastUsed(const QString &selector, const QString &domain);

private:
    explicit DKIMManagerKey(QObject *parent = nullptr);
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QStringList keyRecorderList(KSharedConfig::Ptr &config) const;
    QList<KeyInfo> mKeys;
    QCA::Initializer *const mQcaInitializer;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::KeyInfo, Q_RELOCATABLE_TYPE);
Q_DECLARE_METATYPE(MessageViewer::KeyInfo)
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::KeyInfo &t);
