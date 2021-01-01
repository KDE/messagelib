/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef LOOKUPKEY_H
#define LOOKUPKEY_H

#include <QObject>
class QDnsLookup;
class LookUpKey : public QObject
{
    Q_OBJECT
public:
    explicit LookUpKey(QObject *parent = nullptr);
    ~LookUpKey() = default;
    void lookUpServer(const QString &addr);
private:
    void handleServers();
    QDnsLookup *mDnsLookup = nullptr;
    QString mAddress;
};

#endif // LOOKUPKEY_H
