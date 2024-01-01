/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "findbarbasetest.h"
#include "../findbarbase.h"

#include <PimCommon/LineEditWithCompleterNg>
#include <QLabel>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QToolButton>

FindBarBaseTest::FindBarBaseTest(QObject *parent)
    : QObject(parent)
{
}

FindBarBaseTest::~FindBarBaseTest() = default;

void FindBarBaseTest::shouldHaveDefaultValue()
{
    WebEngineViewer::FindBarBase bar;
    auto status = bar.findChild<QLabel *>(QStringLiteral("status"));
    QVERIFY(status);
    QVERIFY(status->text().isEmpty());

    auto previous = bar.findChild<QPushButton *>(QStringLiteral("findprevious"));
    QVERIFY(previous);
    QVERIFY(!previous->isEnabled());

    auto next = bar.findChild<QPushButton *>(QStringLiteral("findnext"));
    QVERIFY(next);
    QVERIFY(!next->isEnabled());

    auto close = bar.findChild<QToolButton *>(QStringLiteral("close"));
    QVERIFY(close);

    auto lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
    QVERIFY(lineedit);
    QVERIFY(lineedit->text().isEmpty());
}

void FindBarBaseTest::shouldClearLineWhenClose()
{
    WebEngineViewer::FindBarBase bar;
    bar.show();
    bar.activateWindow();
    QSignalSpy spy(&bar, &WebEngineViewer::FindBarBase::hideFindBar);
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    QVERIFY(bar.isVisible());
    bar.focusAndSetCursor();
    auto lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
    lineedit->setText(QStringLiteral("FOO"));
    QVERIFY(!lineedit->text().isEmpty());
    QVERIFY(lineedit->hasFocus());
    bar.closeBar();
    QVERIFY(lineedit->text().isEmpty());
    QVERIFY(!lineedit->hasFocus());
    QCOMPARE(spy.count(), 1);
}

void FindBarBaseTest::shouldEnableDisableNextPreviousButton()
{
    WebEngineViewer::FindBarBase bar;
    bar.show();
    bar.activateWindow();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    auto previous = bar.findChild<QPushButton *>(QStringLiteral("findprevious"));

    auto next = bar.findChild<QPushButton *>(QStringLiteral("findnext"));

    bar.autoSearch(QStringLiteral("FOO"));
    QVERIFY(next->isEnabled());
    QVERIFY(previous->isEnabled());

    bar.autoSearch(QString());
    QVERIFY(!next->isEnabled());
    QVERIFY(!previous->isEnabled());
}

void FindBarBaseTest::shouldClearAllWhenShowBar()
{
    WebEngineViewer::FindBarBase bar;
    bar.show();
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    auto status = bar.findChild<QLabel *>(QStringLiteral("status"));
    status->setText(QStringLiteral("FOO"));
    bar.closeBar();

    bar.show();
    bar.focusAndSetCursor();
    auto lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
    QVERIFY(lineedit->hasFocus());
    QVERIFY(status->text().isEmpty());
}

QTEST_MAIN(FindBarBaseTest)

#include "moc_findbarbasetest.cpp"
