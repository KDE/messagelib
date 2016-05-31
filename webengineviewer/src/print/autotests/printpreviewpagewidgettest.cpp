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

#include "printpreviewpagewidgettest.h"
#include "../printpreviewpagewidget.h"
#include "../printpreviewpageviewer.h"

#include <QDialogButtonBox>
#include <QTest>
#include <QLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

PrintPreviewPageWidgetTest::PrintPreviewPageWidgetTest(QObject *parent)
    : QObject(parent)
{

}

PrintPreviewPageWidgetTest::~PrintPreviewPageWidgetTest()
{

}

void PrintPreviewPageWidgetTest::shouldHaveNotMargin()
{
    WebEngineViewer::PrintPreviewPageWidget w;
    QCOMPARE(w.layout()->margin(), 0);
}

void PrintPreviewPageWidgetTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintPreviewPageWidget w;
    WebEngineViewer::PrintPreviewPageViewer *page = w.findChild<WebEngineViewer::PrintPreviewPageViewer *>(QStringLiteral("printpreviewpage"));
    QVERIFY(page);

    QHBoxLayout *hbox = w.findChild<QHBoxLayout *>(QStringLiteral("layoutcombobox"));
    QVERIFY(hbox);
    QCOMPARE(hbox->margin(), 0);

    QLabel *lab = w.findChild<QLabel *>(QStringLiteral("labelcombobox"));
    QVERIFY(lab);

    QComboBox *mPageComboBox = w.findChild<QComboBox *>(QStringLiteral("pagecombobox"));
    QVERIFY(mPageComboBox);
    QCOMPARE(mPageComboBox->count(), 0);

    QVERIFY(!w.deleteFile());
}

QTEST_MAIN(PrintPreviewPageWidgetTest)
