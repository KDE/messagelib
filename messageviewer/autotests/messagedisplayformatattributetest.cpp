/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagedisplayformatattributetest.h"
#include "../src/viewer/messagedisplayformatattribute.h"
#include <QTest>

MessageDisplayFormatAttributeTest::MessageDisplayFormatAttributeTest(QObject *parent)
    : QObject(parent)
{
}

MessageDisplayFormatAttributeTest::~MessageDisplayFormatAttributeTest() = default;

void MessageDisplayFormatAttributeTest::shouldHaveDefaultValue()
{
    MessageViewer::MessageDisplayFormatAttribute attr;
    QVERIFY(!attr.remoteContent());
    QCOMPARE(attr.messageFormat(), MessageViewer::Viewer::UseGlobalSetting);
}

void MessageDisplayFormatAttributeTest::shouldChangeRemoteValue()
{
    MessageViewer::MessageDisplayFormatAttribute attr;
    attr.setRemoteContent(true);
    QVERIFY(attr.remoteContent());
}

void MessageDisplayFormatAttributeTest::shouldChangeMessageFormat()
{
    MessageViewer::Viewer::DisplayFormatMessage format = MessageViewer::Viewer::Html;
    MessageViewer::MessageDisplayFormatAttribute attr;
    attr.setMessageFormat(format);
    QCOMPARE(attr.messageFormat(), format);

    format = MessageViewer::Viewer::Text;
    attr.setMessageFormat(format);
    QCOMPARE(attr.messageFormat(), format);

    format = MessageViewer::Viewer::UseGlobalSetting;
    attr.setMessageFormat(format);
    QCOMPARE(attr.messageFormat(), format);
}

void MessageDisplayFormatAttributeTest::shouldDeserializeValue()
{
    MessageViewer::Viewer::DisplayFormatMessage format = MessageViewer::Viewer::Html;
    MessageViewer::MessageDisplayFormatAttribute attr;
    attr.setMessageFormat(format);
    attr.setRemoteContent(true);
    const QByteArray ba = attr.serialized();
    MessageViewer::MessageDisplayFormatAttribute result;
    result.deserialize(ba);
    QVERIFY(attr == result);
}

void MessageDisplayFormatAttributeTest::shouldCloneAttribute()
{
    MessageViewer::Viewer::DisplayFormatMessage format = MessageViewer::Viewer::Html;
    MessageViewer::MessageDisplayFormatAttribute attr;
    attr.setMessageFormat(format);
    attr.setRemoteContent(true);
    MessageViewer::MessageDisplayFormatAttribute *result = attr.clone();
    QVERIFY(attr == *result);
    delete result;
}

void MessageDisplayFormatAttributeTest::shouldDefineType()
{
    MessageViewer::MessageDisplayFormatAttribute attr;
    QCOMPARE(attr.type(), QByteArrayLiteral("MessageDisplayFormatAttribute"));
}

QTEST_MAIN(MessageDisplayFormatAttributeTest)

#include "moc_messagedisplayformatattributetest.cpp"
