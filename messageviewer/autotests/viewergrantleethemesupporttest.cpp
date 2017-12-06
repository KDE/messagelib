/*
  Copyright (c) 2017 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "viewergrantleethemesupporttest.h"
#include "messageviewer/viewer.h"

#include <KActionCollection>
#include <QStandardPaths>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QTest>

ViewerGrantleeThemeSupportTest::ViewerGrantleeThemeSupportTest(QObject *parent)
    : QObject(parent)
{
}

ViewerGrantleeThemeSupportTest::~ViewerGrantleeThemeSupportTest()
{
}

void ViewerGrantleeThemeSupportTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);

    // Point the test to our dummy icon theme
    KConfigGroup cg(KSharedConfig::openConfig(), "Icons");
    cg.writeEntry("Theme", "dummyTheme");
    qputenv("XDG_DATA_DIRS", GRANTLEETHEME_DATA_DIR);
}

void ViewerGrantleeThemeSupportTest::shouldUpdateThemeMenu()
{
    MessageViewer::Viewer *viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(
                                                                  this));
    viewer->show();
    QVERIFY(QTest::qWaitForWindowExposed(viewer));
}

QTEST_MAIN(ViewerGrantleeThemeSupportTest)
