/*
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "viewergrantleethemesupporttest.h"
#include "messageviewer/viewer.h"

#include <KActionCollection>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QStandardPaths>
#include <QTest>

ViewerGrantleeThemeSupportTest::ViewerGrantleeThemeSupportTest(QObject *parent)
    : QObject(parent)
{
}

ViewerGrantleeThemeSupportTest::~ViewerGrantleeThemeSupportTest() = default;

void ViewerGrantleeThemeSupportTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);

    // Point the test to our dummy icon theme
    KConfigGroup cg(KSharedConfig::openConfig(), QStringLiteral("Icons"));
    cg.writeEntry("Theme", "dummyTheme");
    qputenv("XDG_DATA_DIRS", GRANTLEETHEME_DATA_DIR);
}

void ViewerGrantleeThemeSupportTest::shouldUpdateThemeMenu()
{
    auto viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(this));
    viewer->show();
    QVERIFY(QTest::qWaitForWindowExposed(viewer));

    delete viewer;
}

QTEST_MAIN(ViewerGrantleeThemeSupportTest)

#include "moc_viewergrantleethemesupporttest.cpp"
