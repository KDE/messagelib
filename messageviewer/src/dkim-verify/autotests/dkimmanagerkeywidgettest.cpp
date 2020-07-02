/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidgettest.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <QTest>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <KTreeWidgetSearchLine>
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
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    KTreeWidgetSearchLine *searchLineEdit = w.findChild<KTreeWidgetSearchLine *>(QStringLiteral("searchlineedit"));
    QVERIFY(searchLineEdit);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
    mainLayout->addWidget(searchLineEdit);

    QTreeWidget *mTreeWidget = w.findChild<QTreeWidget *>(QStringLiteral("treewidget"));
    QVERIFY(mTreeWidget);
    QCOMPARE(mTreeWidget->topLevelItemCount(), 0);
}
