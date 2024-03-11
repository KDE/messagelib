/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "backoffmodemanagertest.h"
#include "../backoffmodemanager.h"
#include <QStandardPaths>
#include <QTest>
QTEST_GUILESS_MAIN(BackOffModeManagerTest)
BackOffModeManagerTest::BackOffModeManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

BackOffModeManagerTest::~BackOffModeManagerTest() = default;

void BackOffModeManagerTest::shouldHaveDefaultValue()
{
    WebEngineViewer::BackOffModeManager *manager = WebEngineViewer::BackOffModeManager::self();
    QVERIFY(!manager->isInBackOffMode());
    QCOMPARE(manager->numberOfHttpFailed(), 0);
}

void BackOffModeManagerTest::shouldStartBackOffMode()
{
    WebEngineViewer::BackOffModeManager *manager = WebEngineViewer::BackOffModeManager::self();
    manager->startOffMode();
    QVERIFY(manager->isInBackOffMode());
}

void BackOffModeManagerTest::shouldIncreaseBackOff()
{
    WebEngineViewer::BackOffModeManager *manager = WebEngineViewer::BackOffModeManager::self();
    for (int i = 0; i < 5; ++i) {
        manager->startOffMode();
    }
    QCOMPARE(manager->numberOfHttpFailed(), 5);
}

#include "moc_backoffmodemanagertest.cpp"
