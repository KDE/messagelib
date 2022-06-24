/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mdnstateattributetest.h"
#include "messagecomposer/mdnstateattribute.h"
#include <QTest>

MDNStateAttributeTest::MDNStateAttributeTest(QObject *parent)
    : QObject(parent)
{
}

MDNStateAttributeTest::~MDNStateAttributeTest() = default;

void MDNStateAttributeTest::shouldHaveDefaultValue()
{
    MessageComposer::MDNStateAttribute attr;
    QCOMPARE(attr.mdnState(), MessageComposer::MDNStateAttribute::MDNStateUnknown);
}

void MDNStateAttributeTest::shouldHaveType()
{
    MessageComposer::MDNStateAttribute attr;
    QCOMPARE(attr.type(), QByteArray("MDNStateAttribute"));
}

void MDNStateAttributeTest::shouldSerializedAttribute()
{
    MessageComposer::MDNStateAttribute attr;

    attr.setMDNState(MessageComposer::MDNStateAttribute::MDNDenied);
    QCOMPARE(attr.mdnState(), MessageComposer::MDNStateAttribute::MDNDenied);
    const QByteArray ba = attr.serialized();
    MessageComposer::MDNStateAttribute result;
    result.deserialize(ba);
    QVERIFY(attr == result);
}

void MDNStateAttributeTest::shouldCloneAttribute()
{
    MessageComposer::MDNStateAttribute attr;
    attr.setMDNState(MessageComposer::MDNStateAttribute::MDNDenied);

    MessageComposer::MDNStateAttribute *result = attr.clone();
    QVERIFY(attr == *result);
    delete result;
}

QTEST_MAIN(MDNStateAttributeTest)
