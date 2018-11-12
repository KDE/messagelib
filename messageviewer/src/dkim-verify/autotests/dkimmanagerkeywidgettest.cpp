/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#include "dkimmanagerkeywidgettest.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <QTest>
#include <QTreeWidget>
#include <QVBoxLayout>
QTEST_MAIN(DKIMManagerKeyWidgetTest)
DKIMManagerKeyWidgetTest::DKIMManagerKeyWidgetTest(QObject *parent)
    : QObject(parent)
{

}

void DKIMManagerKeyWidgetTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMManagerKeyWidget w;

    QVBoxLayout *mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->margin(), 0);

    QTreeWidget *mTreeWidget = w.findChild<QTreeWidget *>(QStringLiteral("treewidget"));
    QVERIFY(mTreeWidget);
    //TODO verify that it's empty
}
