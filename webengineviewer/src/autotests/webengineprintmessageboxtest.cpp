/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "webengineprintmessageboxtest.h"
#include "config-webengineviewer.h"
#include "../webengineprintmessagebox.h"
#include <QTest>
#include <QPushButton>
#include <QSignalSpy>

WebEnginePrintMessageBoxTest::WebEnginePrintMessageBoxTest(QObject *parent)
    : QObject(parent)
{
}

WebEnginePrintMessageBoxTest::~WebEnginePrintMessageBoxTest()
{

}

void WebEnginePrintMessageBoxTest::shouldHaveDefaultValue()
{
    WebEngineViewer::WebEnginePrintMessageBox box;
    QVERIFY(!box.engineView());
    QPushButton *openInBrowser = box.findChild<QPushButton *>(QStringLiteral("openinbrowser"));
    QVERIFY(openInBrowser);

#ifdef WEBENGINEVIEWER_PRINTPREVIEW_SUPPORT
    QPushButton *openInPreviewDialogBox = box.findChild<QPushButton *>(QStringLiteral("openprintpreview"));
    QVERIFY(openInPreviewDialogBox);
#endif
}

void WebEnginePrintMessageBoxTest::shouldEmitOpenPrintPreviewSignal()
{
    WebEngineViewer::WebEnginePrintMessageBox box;
    QSignalSpy spyPrintPreview(&box, SIGNAL(openPrintPreview()));
#ifdef WEBENGINEVIEWER_PRINTPREVIEW_SUPPORT
    QPushButton *openInPreviewDialogBox = box.findChild<QPushButton *>(QStringLiteral("openprintpreview"));
    QTest::mouseClick(openInPreviewDialogBox, Qt::LeftButton);
    QCOMPARE(spyPrintPreview.count(), 1);
#else
    Q_UNUSED(spyPrintPreview);
#endif

}

QTEST_MAIN(WebEnginePrintMessageBoxTest)
