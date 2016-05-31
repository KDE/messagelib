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

#include "printpreviewdialogtest.h"
#include "../printpreviewdialog.h"
#include "../printpreviewpagewidget.h"
#include <QTest>
#include <QDialogButtonBox>
#include <QPushButton>

PrintPreviewDialogTest::PrintPreviewDialogTest(QObject *parent)
    : QObject(parent)
{

}

PrintPreviewDialogTest::~PrintPreviewDialogTest()
{

}

void PrintPreviewDialogTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintPreviewDialog dlg;

    WebEngineViewer::PrintPreviewPageWidget *widget = dlg.findChild<WebEngineViewer::PrintPreviewPageWidget *>(QStringLiteral("printpreviewwidget"));
    QVERIFY(widget);
    QDialogButtonBox *buttonBox = dlg.findChild<QDialogButtonBox *>(QStringLiteral("buttonbox"));
    QVERIFY(buttonBox);

    QPushButton *printButton = buttonBox->findChild<QPushButton *>(QStringLiteral("printbutton"));
    QVERIFY(printButton);
}

QTEST_MAIN(PrintPreviewDialogTest)
