/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidgettest.h"
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
    // TODO
    // TODO
}

#include "moc_searchlinecommandwidgettest.cpp"
