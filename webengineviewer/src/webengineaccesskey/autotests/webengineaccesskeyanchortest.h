/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class WebEngineAccessKeyAnchorTest : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineAccessKeyAnchorTest(QObject *parent = nullptr);
    ~WebEngineAccessKeyAnchorTest() override;
private Q_SLOTS:
    void shouldReturnEmptyAccessKeyAnchor();
};
