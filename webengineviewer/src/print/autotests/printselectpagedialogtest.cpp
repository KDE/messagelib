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

#include "printselectpagedialogtest.h"
#include "../printselectpagewidget.h"
#include "../printselectpagedialog.h"

#include <QTest>
#include <QVBoxLayout>
#include <QDialogButtonBox>

PrintSelectPageDialogTest::PrintSelectPageDialogTest(QObject *parent)
    : QObject(parent)
{

}

PrintSelectPageDialogTest::~PrintSelectPageDialogTest()
{

}

void PrintSelectPageDialogTest::shouldHaveDefaultValue()
{
    WebEngineViewer::PrintSelectPageDialog dlg;
    QVBoxLayout *mainLayout = dlg.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);

    WebEngineViewer::PrintSelectPageWidget *mPrintSelectPage = dlg.findChild<WebEngineViewer::PrintSelectPageWidget *>(QStringLiteral("printselectpage"));
    QVERIFY(mPrintSelectPage);

    QDialogButtonBox *buttonBox = dlg.findChild<QDialogButtonBox *>(QStringLiteral("buttonbox"));
    QVERIFY(buttonBox);
}

QTEST_MAIN(PrintSelectPageDialogTest)
