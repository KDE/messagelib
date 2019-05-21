/*
  Copyright (c) 2016-2019 Montel Laurent <montel@kde.org>

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

#include "searchlinestatustest.h"
#include "../src/core/widgets/searchlinestatus.h"
#include <QMenu>
#include <QTest>
#include <QCompleter>

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
    QMenu *filterMenu = w.findChild<QMenu *>(QStringLiteral("filtermenu"));
    QVERIFY(filterMenu);
    QVERIFY(!filterMenu->actions().isEmpty());

    QVERIFY(w.completer());
    QVERIFY(w.completer()->model());
    QCOMPARE(w.completer()->model()->rowCount(), 0);

    //Verify if qt qlineedit name changed
    QAction *act = w.findChild<QAction *>(QStringLiteral("_q_qlineeditclearaction"));
    QVERIFY(act);
}

void SearchLineStatusTest::shouldAddCompletionItem()
{
    MessageList::Core::SearchLineStatus w;
    w.addCompletionItem(QStringLiteral("ff"));
    QCOMPARE(w.completer()->model()->rowCount(), 1);

    //Don't add same element
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
