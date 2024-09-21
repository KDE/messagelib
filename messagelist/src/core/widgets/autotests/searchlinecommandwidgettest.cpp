/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidgettest.h"
#include "core/widgets/searchlinecommandflowlayout.h"
#include "core/widgets/searchlinecommandwidget.h"
#include <QTest>
QTEST_MAIN(SearchLineCommandWidgetTest)

SearchLineCommandWidgetTest::SearchLineCommandWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void SearchLineCommandWidgetTest::shouldHaveDefaultValues()
{
    MessageList::Core::SearchLineCommandWidget w;

    auto flowLayout = w.findChild<MessageList::Core::SearchLineCommandFlowLayout *>(QStringLiteral("flowLayout"));
    QVERIFY(flowLayout);
    QCOMPARE(flowLayout->contentsMargins(), QMargins{});
    QCOMPARE(flowLayout->spacing(), 0);
    QCOMPARE(flowLayout->count(), 4);
}

#include "moc_searchlinecommandwidgettest.cpp"
