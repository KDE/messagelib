/*
  SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QObject>

class ScamAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit ScamAttributeTest(QObject *parent = nullptr);
    ~ScamAttributeTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAffectValue();
    void shouldDeserializeValue();
    void shouldCloneAttribute();
    void shouldHaveType();
};

