/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "printselectpagewidgettest.h"
#include "../printselectpagewidget.h"
#include <QListWidget>
#include <QTest>
#include <QVBoxLayout>

PrintSelectPageWidgetTest::PrintSelectPageWidgetTest(QObject *parent)
    : QObject(parent)
{

}

PrintSelectPageWidgetTest::~PrintSelectPageWidgetTest()
{

}

void PrintSelectPageWidgetTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintSelectPageWidget w;
    QVBoxLayout *mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->margin(), 0);

    QListWidget *mListPage = w.findChild<QListWidget *>(QStringLiteral("listpage"));
    QVERIFY(mListPage);
    QCOMPARE(mListPage->count(), 0);
}

void PrintSelectPageWidgetTest::shouldHadItems()
{
    WebEngineViewer::PrintSelectPageWidget w;
    QListWidget *mListPage = w.findChild<QListWidget *>(QStringLiteral("listpage"));
    w.setPages(5);
    QCOMPARE(mListPage->count(), 5);
}

void PrintSelectPageWidgetTest::shouldNotAddPages()
{
    WebEngineViewer::PrintSelectPageWidget w;
    QListWidget *mListPage = w.findChild<QListWidget *>(QStringLiteral("listpage"));
    w.setPages(0);
    QCOMPARE(mListPage->count(), 0);
}

QTEST_MAIN(PrintSelectPageWidgetTest)
