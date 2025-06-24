/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchlinecommandwidgettest.h"
using namespace Qt::Literals::StringLiterals;

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

    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mSearchLineCommandButtonsWidget = w.findChild<MessageList::Core::SearchLineCommandButtonsWidget *>(u"mSearchLineCommandButtonsWidget"_s);
    QVERIFY(mSearchLineCommandButtonsWidget);

    auto mLabel = w.findChild<QLabel *>(u"mLabel"_s);
    QVERIFY(mLabel);
    QVERIFY(mLabel->text().isEmpty());
    QVERIFY(mLabel->wordWrap());
}

#include "moc_searchlinecommandwidgettest.cpp"
