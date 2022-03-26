/*
   SPDX-FileCopyrightText: 2020-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class RemoteContentInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteContentInfoTest(QObject *parent = nullptr);
    ~RemoteContentInfoTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldVerifyIsValid();
};
