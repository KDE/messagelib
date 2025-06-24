/*
   SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentconfigurewidgettest.h"
using namespace Qt::Literals::StringLiterals;

#include "remote-content/remotecontentconfigurewidget.h"
#include <KTreeWidgetSearchLine>
#include <QStandardPaths>
#include <QTest>
#include <QVBoxLayout>

QTEST_MAIN(RemoteContentConfigureWidgetTest)
RemoteContentConfigureWidgetTest::RemoteContentConfigureWidgetTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void RemoteContentConfigureWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentConfigureWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(u"mainLayout"_s);
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), QMargins{});

    auto mTreeWidget = w.findChild<QTreeWidget *>(u"treewidget"_s);
    QVERIFY(mTreeWidget);
    QVERIFY(mTreeWidget->alternatingRowColors());
    QVERIFY(!mTreeWidget->rootIsDecorated());

    auto searchLineEdit = w.findChild<KTreeWidgetSearchLine *>(u"searchlineedit"_s);
    QVERIFY(searchLineEdit);
    QVERIFY(searchLineEdit->isClearButtonEnabled());
}

#include "moc_remotecontentconfigurewidgettest.cpp"
