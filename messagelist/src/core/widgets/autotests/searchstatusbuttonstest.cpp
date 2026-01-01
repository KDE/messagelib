/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchstatusbuttonstest.h"
using namespace Qt::Literals::StringLiterals;

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

    auto mButtonGroup = w.findChild<QButtonGroup *>(u"mButtonGroup"_s);
    QVERIFY(mButtonGroup);
    QVERIFY(!mButtonGroup->exclusive());

    auto mMainLayout = w.findChild<QHBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mMainLayout);
    QCOMPARE(mMainLayout->spacing(), 0);
    QCOMPARE(mMainLayout->contentsMargins(), QMargins{});

    QCOMPARE(mButtonGroup->buttons().count(), 11);
}

#include "moc_searchstatusbuttonstest.cpp"
