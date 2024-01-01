/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include "scamattributetest.h"
#include "../scamattribute.h"
#include <QTest>
ScamAttributeTest::ScamAttributeTest(QObject *parent)
    : QObject(parent)
{
}

ScamAttributeTest::~ScamAttributeTest() = default;

void ScamAttributeTest::shouldHaveDefaultValue()
{
    MessageViewer::ScamAttribute attr;
    QVERIFY(!attr.isAScam());
}

void ScamAttributeTest::shouldAffectValue()
{
    MessageViewer::ScamAttribute attr;
    bool isScam = false;
    attr.setIsAScam(isScam);
    QCOMPARE(attr.isAScam(), isScam);
    isScam = true;
    attr.setIsAScam(isScam);
    QCOMPARE(attr.isAScam(), isScam);
}

void ScamAttributeTest::shouldDeserializeValue()
{
    MessageViewer::ScamAttribute attr;
    const bool isScam = true;
    attr.setIsAScam(isScam);
    const QByteArray ba = attr.serialized();
    MessageViewer::ScamAttribute result;
    result.deserialize(ba);
    QVERIFY(result == attr);
}

void ScamAttributeTest::shouldCloneAttribute()
{
    MessageViewer::ScamAttribute attr;
    const bool isScam = true;
    attr.setIsAScam(isScam);
    MessageViewer::ScamAttribute *cloneAttr = attr.clone();
    QCOMPARE(attr.isAScam(), cloneAttr->isAScam());
    delete cloneAttr;
}

void ScamAttributeTest::shouldHaveType()
{
    MessageViewer::ScamAttribute attr;
    QCOMPARE(attr.type(), QByteArray("ScamAttribute"));
}

QTEST_MAIN(ScamAttributeTest)

#include "moc_scamattributetest.cpp"
