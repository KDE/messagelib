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

#ifndef DKIMMANAGERKEY_H
#define DKIMMANAGERKEY_H

#include <QObject>
#include "messageviewer_export.h"
#include "config-messageviewer.h"
#include <QVector>
#ifdef USE_DKIM_CHECKER
namespace QCA {
class Initializer;
}
#endif
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
    QVector<KeyInfo> mKeys;
#ifdef USE_DKIM_CHECKER
    QCA::Initializer *mQcaInitializer = nullptr;
#endif
};
}
Q_DECLARE_TYPEINFO(MessageViewer::KeyInfo, Q_MOVABLE_TYPE);
#endif // DKIMMANAGERKEY_H
