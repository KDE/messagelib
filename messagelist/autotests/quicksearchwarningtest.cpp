/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "quicksearchwarningtest.h"
#include "../src/core/widgets/quicksearchwarning.h"
#include <QAction>
#include <QTest>
QuickSearchWarningTest::QuickSearchWarningTest(QObject *parent)
    : QObject(parent)
{
}

QuickSearchWarningTest::~QuickSearchWarningTest()
{
}

void QuickSearchWarningTest::shouldHaveDefaultValue()
{
    MessageList::Core::QuickSearchWarning w;
    QVERIFY(!w.isVisible());
    auto act = w.findChild<QAction *>(QStringLiteral("donotshowagain"));
    QVERIFY(act);
}

void QuickSearchWarningTest::shouldSetVisible()
{
    MessageList::Core::QuickSearchWarning w;
    w.setSearchText(QStringLiteral("1"));
    QVERIFY(w.isVisible());
}

void QuickSearchWarningTest::shouldSetSearchText()
{
    QFETCH(QString, input);
    QFETCH(bool, visible);
    MessageList::Core::QuickSearchWarning w;
    w.setSearchText(input);
    QCOMPARE(w.isVisible(), visible);
}

void QuickSearchWarningTest::shouldSetSearchText_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("visible");
    QTest::newRow("bigword") << QStringLiteral("foofoofoo") << false;
    QTest::newRow("1character") << QStringLiteral("f") << true;
    QTest::newRow("multibigword") << QStringLiteral("foo foo foo") << false;
    QTest::newRow("multibigwordwithasmallone") << QStringLiteral("foo foo foo 1") << true;
    QTest::newRow("aspace") << QStringLiteral(" ") << false;
    QTest::newRow("multispace") << QStringLiteral("            ") << false;
}

QTEST_MAIN(QuickSearchWarningTest)
