/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHECKRSAPUBLICKEY_H
#define CHECKRSAPUBLICKEY_H

#include <QObject>

class CheckRSAPublicKey : public QObject
{
    Q_OBJECT
public:
    explicit CheckRSAPublicKey(QObject *parent = nullptr);
};

#endif // CHECKRSAPUBLICKEY_H
