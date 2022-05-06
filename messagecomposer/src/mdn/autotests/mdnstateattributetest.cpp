/*
  SPDX-FileCopyrightText: 2014-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mdnstateattributetest.h"
#include "../mdnstateattribute.h"
#include <QTest>

MDNStateAttributeTest::MDNStateAttributeTest(QObject *parent)
    : QObject(parent)
{
}

MDNStateAttributeTest::~MDNStateAttributeTest() = default;

void MDNStateAttributeTest::shouldHaveDefaultValue()
{
    MailCommon::MDNStateAttribute attr;
    QCOMPARE(attr.mdnState(), MailCommon::MDNStateAttribute::MDNStateUnknown);
}

void MDNStateAttributeTest::shouldHaveType()
{
    MailCommon::MDNStateAttribute attr;
    QCOMPARE(attr.type(), QByteArray("MDNStateAttribute"));
}

void MDNStateAttributeTest::shouldSerializedAttribute()
{
    MailCommon::MDNStateAttribute attr;

    attr.setMDNState(MailCommon::MDNStateAttribute::MDNDenied);
    QCOMPARE(attr.mdnState(), MailCommon::MDNStateAttribute::MDNDenied);
    const QByteArray ba = attr.serialized();
    MailCommon::MDNStateAttribute result;
    result.deserialize(ba);
    QVERIFY(attr == result);
}

void MDNStateAttributeTest::shouldCloneAttribute()
{
    MailCommon::MDNStateAttribute attr;
    attr.setMDNState(MailCommon::MDNStateAttribute::MDNDenied);

    MailCommon::MDNStateAttribute *result = attr.clone();
    QVERIFY(attr == *result);
    delete result;
}

QTEST_MAIN(MDNStateAttributeTest)
