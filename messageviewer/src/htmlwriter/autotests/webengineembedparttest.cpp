/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "webengineembedparttest.h"
#include "../webengineembedpart.h"
#include <QTest>

WebEngineEmbedPartTest::WebEngineEmbedPartTest(QObject *parent)
    : QObject(parent)
{
}

WebEngineEmbedPartTest::~WebEngineEmbedPartTest() = default;

void WebEngineEmbedPartTest::shouldHaveDefaultValue()
{
    MessageViewer::WebEngineEmbedPart part;
    QVERIFY(part.isEmpty());
}

void WebEngineEmbedPartTest::shouldClearValue()
{
    MessageViewer::WebEngineEmbedPart part;
    part.addEmbedPart(QByteArrayLiteral("foo"), QStringLiteral("bla"));
    QVERIFY(!part.isEmpty());
    QCOMPARE(part.embeddedPartMap().count(), 1);

    part.clear();
    QVERIFY(part.isEmpty());
}

void WebEngineEmbedPartTest::shouldAddValues()
{
    MessageViewer::WebEngineEmbedPart part;
    part.addEmbedPart(QByteArrayLiteral("foo"), QStringLiteral("bla"));
    QVERIFY(!part.isEmpty());
    QCOMPARE(part.embeddedPartMap().count(), 1);
    part.addEmbedPart(QByteArrayLiteral("foo1"), QStringLiteral("bla"));
    QCOMPARE(part.embeddedPartMap().count(), 2);
}

void WebEngineEmbedPartTest::shouldAddTwoIdenticalValues()
{
    MessageViewer::WebEngineEmbedPart part;
    part.addEmbedPart(QByteArrayLiteral("foo"), QStringLiteral("bla"));
    QVERIFY(!part.isEmpty());
    QCOMPARE(part.embeddedPartMap().count(), 1);
    part.addEmbedPart(QByteArrayLiteral("foo"), QStringLiteral("bla"));
    QCOMPARE(part.embeddedPartMap().count(), 1);
}

QTEST_MAIN(WebEngineEmbedPartTest)

#include "moc_webengineembedparttest.cpp"
