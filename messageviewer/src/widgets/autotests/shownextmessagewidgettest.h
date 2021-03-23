/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QObject>

class ShowNextMessageWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit ShowNextMessageWidgetTest(QObject *parent = nullptr);
    ~ShowNextMessageWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

