/*
  SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "quicksearchwarningtest.h"
using namespace Qt::Literals::StringLiterals;

#include "../src/core/widgets/quicksearchwarning.h"
#include <QAction>
#include <QTest>
QuickSearchWarningTest::QuickSearchWarningTest(QObject *parent)
    : QObject(parent)
{
}

QuickSearchWarningTest::~QuickSearchWarningTest() = default;

void QuickSearchWarningTest::shouldHaveDefaultValue()
{
    MessageList::Core::QuickSearchWarning w;
    QVERIFY(!w.isVisible());
    auto act = w.findChild<QAction *>(u"donotshowagain"_s);
    QVERIFY(act);
}

void QuickSearchWarningTest::shouldSetVisible()
{
    MessageList::Core::QuickSearchWarning w;
    w.setSearchText(u"1"_s);
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
    QTest::newRow("bigword") << u"foofoofoo"_s << false;
    QTest::newRow("1character") << u"f"_s << true;
    QTest::newRow("multibigword") << u"foo foo foo"_s << false;
    QTest::newRow("multibigwordwithasmallone") << u"foo foo foo 1"_s << true;
    QTest::newRow("aspace") << u" "_s << false;
    QTest::newRow("multispace") << u"            "_s << false;
}

QTEST_MAIN(QuickSearchWarningTest)

#include "moc_quicksearchwarningtest.cpp"
