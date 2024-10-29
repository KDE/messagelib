/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchlinecommandwidgettest.h"
#include "core/widgets/searchlinecommandbuttonswidget.h"
#include "core/widgets/searchlinecommandwidget.h"
#include <QLabel>
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(SearchLineCommandWidgetTest)

SearchLineCommandWidgetTest::SearchLineCommandWidgetTest(QObject *parent)
    : QObject{parent}
{
}

void SearchLineCommandWidgetTest::shouldHaveDefaultValues()
{
    MessageList::Core::SearchLineCommandWidget w;

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mSearchLineCommandButtonsWidget = w.findChild<MessageList::Core::SearchLineCommandButtonsWidget *>(QStringLiteral("mSearchLineCommandButtonsWidget"));
    QVERIFY(mSearchLineCommandButtonsWidget);

    auto mLabel = w.findChild<QLabel *>(QStringLiteral("mLabel"));
    QVERIFY(mLabel);
    QVERIFY(mLabel->text().isEmpty());
}

#include "moc_searchlinecommandwidgettest.cpp"
