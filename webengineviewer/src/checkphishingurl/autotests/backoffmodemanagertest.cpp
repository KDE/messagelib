/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "backoffmodemanagertest.h"
#include "../backoffmodemanager.h"
#include "../checkphishingurlutil.h"

#include <KConfig>

#include <QStandardPaths>
#include <QTest>
QTEST_GUILESS_MAIN(BackOffModeManagerTest)
BackOffModeManagerTest::BackOffModeManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);

    KConfig config(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    config.deleteGroup(QStringLiteral("BackOffMode"));
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
    const auto prevCount = manager->numberOfHttpFailed();
    for (int i = 0; i < 5; ++i) {
        manager->startOffMode();
    }
    QCOMPARE(manager->numberOfHttpFailed(), prevCount + 5);
}

#include "moc_backoffmodemanagertest.cpp"
