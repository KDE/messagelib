/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class MessageDisplayFormatAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageDisplayFormatAttributeTest(QObject *parent = nullptr);
    ~MessageDisplayFormatAttributeTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldChangeRemoteValue();
    void shouldChangeMessageFormat();
    void shouldDeserializeValue();
    void shouldCloneAttribute();
    void shouldDefineType();
};
