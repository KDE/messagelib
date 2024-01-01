/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DMarcLookUpKey : public QObject
{
    Q_OBJECT
public:
    explicit DMarcLookUpKey(QObject *parent = nullptr);
    ~DMarcLookUpKey() override;
    void lookUpDomain(const QString &domain);
};
