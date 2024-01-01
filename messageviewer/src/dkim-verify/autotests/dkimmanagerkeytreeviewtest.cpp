/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeytreeviewtest.h"
#include "dkim-verify/dkimmanagerkeytreeview.h"
#include <QTest>
QTEST_MAIN(DKIMManagerKeyTreeViewTest)
DKIMManagerKeyTreeViewTest::DKIMManagerKeyTreeViewTest(QObject *parent)
    : QObject{parent}
{
}

void DKIMManagerKeyTreeViewTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMManagerKeyTreeView w;
    QVERIFY(w.uniformRowHeights());
    QVERIFY(w.alternatingRowColors());
    QVERIFY(!w.rootIsDecorated());
    QCOMPARE(w.selectionBehavior(), QAbstractItemView::SelectRows);
    QCOMPARE(w.selectionMode(), QAbstractItemView::ExtendedSelection);
}

#include "moc_dkimmanagerkeytreeviewtest.cpp"
