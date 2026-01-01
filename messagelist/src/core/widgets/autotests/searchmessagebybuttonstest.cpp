/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchmessagebybuttonstest.h"
using namespace Qt::Literals::StringLiterals;

#include "core/widgets/searchmessagebybuttons.h"
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QTest>
QTEST_MAIN(SearchMessageByButtonsTest)
using namespace MessageList::Core;
SearchMessageByButtonsTest::SearchMessageByButtonsTest(QObject *parent)
    : QObject{parent}
{
}

void SearchMessageByButtonsTest::shouldHaveDefaultValues()
{
    SearchMessageByButtons w;
    auto mButtonGroup = w.findChild<QButtonGroup *>(u"mButtonGroup"_s);
    QVERIFY(mButtonGroup);
    QVERIFY(mButtonGroup->exclusive());

    auto mMainLayout = w.findChild<QHBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mMainLayout);
    QCOMPARE(mMainLayout->spacing(), 0);
    QCOMPARE(mMainLayout->contentsMargins(), QMargins{});
}

#include "moc_searchmessagebybuttonstest.cpp"
