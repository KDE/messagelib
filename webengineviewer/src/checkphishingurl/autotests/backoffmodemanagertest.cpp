/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "backoffmodemanagertest.h"
#include "../backoffmodemanager.h"
#include <QStandardPaths>
#include <QTest>
BackOffModeManagerTest::BackOffModeManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

BackOffModeManagerTest::~BackOffModeManagerTest()
{
}

void BackOffModeManagerTest::shouldHaveDefaultValue()
{
    WebEngineViewer::BackOffModeManager manager;
    QVERIFY(!manager.isInBackOffMode());
    QCOMPARE(manager.numberOfHttpFailed(), 0);
}

void BackOffModeManagerTest::shouldStartBackOffMode()
{
    WebEngineViewer::BackOffModeManager manager;
    manager.startOffMode();
    QVERIFY(manager.isInBackOffMode());
}

void BackOffModeManagerTest::shouldIncreaseBackOff()
{
    WebEngineViewer::BackOffModeManager manager;
    for (int i = 0; i < 5; ++i) {
        manager.startOffMode();
    }
    QCOMPARE(manager.numberOfHttpFailed(), 5);
}

QTEST_GUILESS_MAIN(BackOffModeManagerTest)
