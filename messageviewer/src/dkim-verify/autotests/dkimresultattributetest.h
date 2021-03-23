/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMResultAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMResultAttributeTest(QObject *parent = nullptr);
    ~DKIMResultAttributeTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldCloneAttribute();
    void shouldHaveType();
    void shouldDeserializeValue();
};

