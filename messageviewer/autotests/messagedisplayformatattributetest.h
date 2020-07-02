/*
  SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEDISPLAYFORMATATTRIBUTETEST_H
#define MESSAGEDISPLAYFORMATATTRIBUTETEST_H

#include <QObject>

class MessageDisplayFormatAttributeTest : public QObject
{
    Q_OBJECT
public:
    explicit MessageDisplayFormatAttributeTest(QObject *parent = nullptr);
    ~MessageDisplayFormatAttributeTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldChangeRemoteValue();
    void shouldChangeMessageFormat();
    void shouldDeserializeValue();
    void shouldCloneAttribute();
    void shouldDefineType();
};

#endif // MESSAGEDISPLAYFORMATATTRIBUTETEST_H
