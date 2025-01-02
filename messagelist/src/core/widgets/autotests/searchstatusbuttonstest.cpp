/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchstatusbuttonstest.h"
#include "core/widgets/searchstatusbuttons.h"
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QTest>
QTEST_MAIN(SearchStatusButtonsTest)
using namespace MessageList::Core;
SearchStatusButtonsTest::SearchStatusButtonsTest(QObject *parent)
    : QObject{parent}
{
}

void SearchStatusButtonsTest::shouldHaveDefaultValues()
{
    SearchStatusButtons w;

    auto mButtonGroup = w.findChild<QButtonGroup *>(QStringLiteral("mButtonGroup"));
    QVERIFY(mButtonGroup);
    QVERIFY(!mButtonGroup->exclusive());

    auto mMainLayout = w.findChild<QHBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mMainLayout);
    QCOMPARE(mMainLayout->spacing(), 0);
    QCOMPARE(mMainLayout->contentsMargins(), QMargins{});

    QCOMPARE(mButtonGroup->buttons().count(), 11);
}

#include "moc_searchstatusbuttonstest.cpp"
