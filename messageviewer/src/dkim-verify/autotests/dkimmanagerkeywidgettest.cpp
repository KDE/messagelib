/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidgettest.h"
#include "dkim-verify/dkimmanagerkeytreeview.h"
#include "dkim-verify/dkimmanagerkeywidget.h"
#include <QLineEdit>
#include <QTest>
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

    auto searchLineEdit = w.findChild<QLineEdit *>(QStringLiteral("searchlineedit"));
    QVERIFY(searchLineEdit);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
    mainLayout->addWidget(searchLineEdit);

    auto mDKIMManagerKeyTreeView = w.findChild<MessageViewer::DKIMManagerKeyTreeView *>(QStringLiteral("mDKIMManagerKeyTreeView"));
    QVERIFY(mDKIMManagerKeyTreeView);
}

#include "moc_dkimmanagerkeywidgettest.cpp"
