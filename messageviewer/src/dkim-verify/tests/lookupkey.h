/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
class QDnsLookup;
class LookUpKey : public QObject
{
    Q_OBJECT
public:
    explicit LookUpKey(QObject *parent = nullptr);
    ~LookUpKey() override = default;
    void lookUpServer(const QString &addr);

private:
    void handleServers();
    QDnsLookup *mDnsLookup = nullptr;
    QString mAddress;
};
