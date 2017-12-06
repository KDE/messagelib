/*
   Copyright (C) 2014-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "findbarbasetest.h"
#include "../findbarbase.h"

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <qtest.h>
#include <QSignalSpy>
#include <PimCommon/LineEditWithCompleterNg>

FindBarBaseTest::FindBarBaseTest(QObject *parent)
    : QObject(parent)
{
}

FindBarBaseTest::~FindBarBaseTest()
{
}

void FindBarBaseTest::shouldHaveDefaultValue()
{
    WebEngineViewer::FindBarBase bar;
    QLabel *status = bar.findChild<QLabel *>(QStringLiteral("status"));
    QVERIFY(status);
    QVERIFY(status->text().isEmpty());

    QPushButton *previous = bar.findChild<QPushButton *>(QStringLiteral("findprevious"));
    QVERIFY(previous);
    QVERIFY(!previous->isEnabled());

    QPushButton *next = bar.findChild<QPushButton *>(QStringLiteral("findnext"));
    QVERIFY(next);
    QVERIFY(!next->isEnabled());

    QToolButton *close = bar.findChild<QToolButton *>(QStringLiteral("close"));
    QVERIFY(close);

    PimCommon::LineEditWithCompleterNg *lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
    QVERIFY(lineedit);
    QVERIFY(lineedit->text().isEmpty());
}

void FindBarBaseTest::shouldClearLineWhenClose()
{
    WebEngineViewer::FindBarBase bar;
    bar.show();
    QApplication::setActiveWindow(&bar);
    QSignalSpy spy(&bar, &WebEngineViewer::FindBarBase::hideFindBar);
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    QVERIFY(bar.isVisible());
    bar.focusAndSetCursor();
    PimCommon::LineEditWithCompleterNg *lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
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
    QApplication::setActiveWindow(&bar);
    QVERIFY(QTest::qWaitForWindowExposed(&bar));
    QPushButton *previous = bar.findChild<QPushButton *>(QStringLiteral("findprevious"));

    QPushButton *next = bar.findChild<QPushButton *>(QStringLiteral("findnext"));

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
    QLabel *status = bar.findChild<QLabel *>(QStringLiteral("status"));
    status->setText(QStringLiteral("FOO"));
    bar.closeBar();

    bar.show();
    bar.focusAndSetCursor();
    PimCommon::LineEditWithCompleterNg *lineedit = bar.findChild<PimCommon::LineEditWithCompleterNg *>(QStringLiteral("searchline"));
    QVERIFY(lineedit->hasFocus());
    QVERIFY(status->text().isEmpty());
}

QTEST_MAIN(FindBarBaseTest)
