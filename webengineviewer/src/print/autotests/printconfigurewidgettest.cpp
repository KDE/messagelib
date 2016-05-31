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

#include "printconfigurewidgettest.h"
#include "../printconfigurewidget.h"
#include <QTest>
#include <QLayout>
#include <QLabel>
#include <QToolButton>

PrintConfigureWidgetTest::PrintConfigureWidgetTest(QObject *parent)
    : QObject(parent)
{

}

PrintConfigureWidgetTest::~PrintConfigureWidgetTest()
{

}

void PrintConfigureWidgetTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintConfigureWidget w;
    QCOMPARE(w.layout()->margin(), 0);

    QLabel *printLayoutLabel = w.findChild<QLabel *>(QStringLiteral("printlayoutlabel"));
    QVERIFY(printLayoutLabel);
    QVERIFY(!printLayoutLabel->text().isEmpty());

    QVBoxLayout *printLayout =  w.findChild<QVBoxLayout *>(QStringLiteral("printLayout"));
    QVERIFY(printLayout);
    QCOMPARE(printLayout->margin(), 0);

    QToolButton *selectPrintLayout = w.findChild<QToolButton *>(QStringLiteral("selectprintlayout"));
    QVERIFY(selectPrintLayout);

    QVERIFY(w.currentPageLayout().isValid());
}

QTEST_MAIN(PrintConfigureWidgetTest)
