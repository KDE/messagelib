/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "searchlinestatustest.h"
#include "../src/core/widgets/searchlinestatus.h"
#include <QCompleter>
#include <QMenu>
#include <QTest>
QTEST_MAIN(SearchLineStatusTest)
SearchLineStatusTest::SearchLineStatusTest(QObject *parent)
    : QObject(parent)
{
}

SearchLineStatusTest::~SearchLineStatusTest() = default;

void SearchLineStatusTest::shouldHaveDefaultValue()
{
    MessageList::Core::SearchLineStatus w;
    QVERIFY(!w.locked());

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

#include "moc_searchlinestatustest.cpp"
