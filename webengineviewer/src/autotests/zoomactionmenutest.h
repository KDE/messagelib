/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ZoomActionMenuTest : public QObject
{
    Q_OBJECT
public:
    explicit ZoomActionMenuTest(QObject *parent = nullptr);
    ~ZoomActionMenuTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAssignZoomFactor();
};
