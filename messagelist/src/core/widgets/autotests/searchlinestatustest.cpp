/*
  SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "searchlinestatustest.h"
using namespace Qt::Literals::StringLiterals;

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
    auto act = w.findChild<QAction *>(u"_q_qlineeditclearaction"_s);
    QVERIFY(act);
}

void SearchLineStatusTest::shouldAddCompletionItem()
{
    MessageList::Core::SearchLineStatus w;
    w.addCompletionItem(u"ff"_s);
    QCOMPARE(w.completer()->model()->rowCount(), 26);

    // Don't add same element
    w.addCompletionItem(u"ff"_s);
    QCOMPARE(w.completer()->model()->rowCount(), 26);

    w.addCompletionItem(u"ffss"_s);
    QCOMPARE(w.completer()->model()->rowCount(), 27);
}

void SearchLineStatusTest::shouldClearCompleter()
{
    MessageList::Core::SearchLineStatus w;
    for (int i = 0; i < 10; ++i) {
        w.addCompletionItem(u"ff%1"_s.arg(i));
    }
    QCOMPARE(w.completer()->model()->rowCount(), 35);
    w.slotClearHistory();
    QCOMPARE(w.completer()->model()->rowCount(), 25);
}

#include "moc_searchlinestatustest.cpp"
