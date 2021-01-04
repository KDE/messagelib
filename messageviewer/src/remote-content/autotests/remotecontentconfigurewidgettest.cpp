/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentconfigurewidgettest.h"
#include "remote-content/remotecontentconfigurewidget.h"
#include <QListWidget>
#include <QTest>
#include <QVBoxLayout>
#include <QStandardPaths>

QTEST_MAIN(RemoteContentConfigureWidgetTest)
RemoteContentConfigureWidgetTest::RemoteContentConfigureWidgetTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void RemoteContentConfigureWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentConfigureWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), {});


    auto mListWidget = w.findChild<QListWidget *>(QStringLiteral("mListWidget"));
    QVERIFY(mListWidget);
}
