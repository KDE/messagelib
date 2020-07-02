/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DMARCLOOKUPKEY_H
#define DMARCLOOKUPKEY_H

#include <QObject>

class DMarcLookUpKey : public QObject
{
    Q_OBJECT
public:
    explicit DMarcLookUpKey(QObject *parent = nullptr);
    ~DMarcLookUpKey();
    void lookUpDomain(const QString &domain);
};

#endif // DMARCLOOKUPKEY_H
