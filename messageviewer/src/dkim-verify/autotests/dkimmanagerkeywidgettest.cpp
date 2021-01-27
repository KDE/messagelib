/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidgettest.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <KTreeWidgetSearchLine>
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

    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainlayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto searchLineEdit = w.findChild<KTreeWidgetSearchLine *>(QStringLiteral("searchlineedit"));
    QVERIFY(searchLineEdit);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
    mainLayout->addWidget(searchLineEdit);

    auto mTreeWidget = w.findChild<QTreeWidget *>(QStringLiteral("treewidget"));
    QVERIFY(mTreeWidget);
    QCOMPARE(mTreeWidget->topLevelItemCount(), 0);
}
