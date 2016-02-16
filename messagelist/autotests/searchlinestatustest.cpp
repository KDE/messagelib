/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

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

}

QTEST_MAIN(SearchLineStatusTest)
