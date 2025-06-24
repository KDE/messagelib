/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidgettest.h"
using namespace Qt::Literals::StringLiterals;

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

    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainlayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins(0, 0, 0, 0));

    auto searchLineEdit = w.findChild<QLineEdit *>(u"searchlineedit"_s);
    QVERIFY(searchLineEdit);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
    mainLayout->addWidget(searchLineEdit);

    auto mDKIMManagerKeyTreeView = w.findChild<MessageViewer::DKIMManagerKeyTreeView *>(u"mDKIMManagerKeyTreeView"_s);
    QVERIFY(mDKIMManagerKeyTreeView);
}

#include "moc_dkimmanagerkeywidgettest.cpp"
