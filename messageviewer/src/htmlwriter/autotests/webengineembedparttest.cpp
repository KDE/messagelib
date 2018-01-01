/*
  Copyright (c) 2016-2018 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webengineembedparttest.h"
#include "../webengineembedpart.h"
#include <QTest>

WebEngineEmbedPartTest::WebEngineEmbedPartTest(QObject *parent)
    : QObject(parent)
{
}

WebEngineEmbedPartTest::~WebEngineEmbedPartTest()
{
}

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
