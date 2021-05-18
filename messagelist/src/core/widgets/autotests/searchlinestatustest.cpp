/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "searchlinestatustest.h"
#include "../src/core/widgets/searchlinestatus.h"
#include <QCompleter>
#include <QMenu>
#include <QTest>

SearchLineStatusTest::SearchLineStatusTest(QObject *parent)
    : QObject(parent)
{
}

SearchLineStatusTest::~SearchLineStatusTest()
{
}

void SearchLineStatusTest::shouldHaveDefaultValue()
{
    MessageList::Core::SearchLineStatus w;
    QVERIFY(!w.containsOutboundMessages());
    QVERIFY(!w.locked());
    auto filterMenu = w.findChild<QMenu *>(QStringLiteral("filtermenu"));
    QVERIFY(filterMenu);
    QVERIFY(!filterMenu->actions().isEmpty());

    QVERIFY(w.completer());
    QVERIFY(w.completer()->model());
    QCOMPARE(w.completer()->model()->rowCount(), 0);

    // Verify if qt qlineedit name changed
    auto act = w.findChild<QAction *>(QStringLiteral("_q_qlineeditclearaction"));
    QVERIFY(act);
}

void SearchLineStatusTest::shouldAddCompletionItem()
{
    MessageList::Core::SearchLineStatus w;
    w.addCompletionItem(QStringLiteral("ff"));
    QCOMPARE(w.completer()->model()->rowCount(), 1);

    // Don't add same element
    w.addCompletionItem(QStringLiteral("ff"));
    QCOMPARE(w.completer()->model()->rowCount(), 1);

    w.addCompletionItem(QStringLiteral("ffss"));
    QCOMPARE(w.completer()->model()->rowCount(), 2);
}

void SearchLineStatusTest::shouldClearCompleter()
{
    MessageList::Core::SearchLineStatus w;
    for (int i = 0; i < 10; ++i) {
        w.addCompletionItem(QStringLiteral("ff%1").arg(i));
    }
    QCOMPARE(w.completer()->model()->rowCount(), 10);
    w.slotClearHistory();
    QCOMPARE(w.completer()->model()->rowCount(), 0);
}

QTEST_MAIN(SearchLineStatusTest)
