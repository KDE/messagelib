/*
  SPDX-FileCopyrightText: 2011 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../src/core/messageitem_p.h"
#include <QDebug>
#include <QObject>
#include <QTest>

using namespace MessageList::Core;

class ItemSizeTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void testItemSize()
    {
        qDebug() << sizeof(Item);
        QVERIFY(sizeof(Item) <= 16);
        qDebug() << sizeof(ItemPrivate);
        QVERIFY(sizeof(ItemPrivate) <= 184);
        qDebug() << sizeof(MessageItem);
        QVERIFY(sizeof(MessageItem) <= 32);
        qDebug() << sizeof(MessageItemPrivate);
        QVERIFY(sizeof(MessageItemPrivate) <= 304);
    }
};

QTEST_GUILESS_MAIN(ItemSizeTest)

#include "itemsizetest.moc"
