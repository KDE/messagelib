/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERKEY_H
#define DKIMMANAGERKEY_H

#include <QObject>
#include "messageviewer_export.h"
#include "config-messageviewer.h"
#include <QVector>
#include <KSharedConfig>
namespace QCA {
class Initializer;
}
namespace MessageViewer {
/**
 * @brief The KeyInfo struct
 * @author Laurent Montel <montel@kde.org>
 */
struct MESSAGEVIEWER_EXPORT KeyInfo {
    KeyInfo()
    {
    }

    KeyInfo(const QString &key, const QString &sel, const QString &dom)
        : keyValue(key)
        , selector(sel)
        , domain(dom)
    {
    }

    QString keyValue;
    QString selector;
    QString domain;
    Q_REQUIRED_RESULT bool operator ==(const KeyInfo &) const;
    Q_REQUIRED_RESULT bool operator !=(const KeyInfo &) const;
};

/**
 * @brief The DKIMManagerKey class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerKey : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerKey(QObject *parent = nullptr);
    ~DKIMManagerKey() override;

    static DKIMManagerKey *self();

    void loadKeys();
    void saveKeys(const QVector<KeyInfo> &lst);

    void addKey(const KeyInfo &key);
    void removeKey(const QString &key);

    Q_REQUIRED_RESULT QVector<KeyInfo> keys() const;

    void saveKeys();
    Q_REQUIRED_RESULT QString keyValue(const QString &selector, const QString &domain);
private:
    Q_REQUIRED_RESULT QStringList keyRecorderList(KSharedConfig::Ptr &config) const;
    QVector<KeyInfo> mKeys;
    QCA::Initializer *mQcaInitializer = nullptr;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::KeyInfo, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator <<(QDebug d, const MessageViewer::KeyInfo &t);
#endif // DKIMMANAGERKEY_H
