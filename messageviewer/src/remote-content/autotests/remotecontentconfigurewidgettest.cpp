/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentconfigurewidgettest.h"
#include "remote-content/remotecontentconfigurewidget.h"
#include <QTest>
#include <QVBoxLayout>
QTEST_MAIN(RemoteContentConfigureWidgetTest)
RemoteContentConfigureWidgetTest::RemoteContentConfigureWidgetTest(QObject *parent)
    : QObject(parent)
{

}

void RemoteContentConfigureWidgetTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentConfigureWidget w;
    auto mainLayout = w.findChild<QVBoxLayout *>(QStringLiteral("mainLayout"));
    QVERIFY(mainLayout);
    QCOMPARE(mainLayout->contentsMargins(), {});
}
