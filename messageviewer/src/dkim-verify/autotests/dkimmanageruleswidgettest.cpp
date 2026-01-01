/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanageruleswidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimmanageruleswidget.h"

#include <KTreeWidgetSearchLine>

#include <QTest>
#include <QTreeWidget>
#include <QVBoxLayout>

QTEST_MAIN(DKIMManageRulesWidgetTest)

DKIMManageRulesWidgetTest::DKIMManageRulesWidgetTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMManageRulesWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMManageRulesWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto mTreeWidget = w.findChild<QTreeWidget *>(u"treewidget"_s);
    QVERIFY(mTreeWidget);
    QVERIFY(!mTreeWidget->rootIsDecorated());
    QVERIFY(mTreeWidget->alternatingRowColors());

    auto searchLineEdit = w.findChild<KTreeWidgetSearchLine *>(u"searchlineedit"_s);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
}

#include "moc_dkimmanageruleswidgettest.cpp"
