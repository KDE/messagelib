/*
  SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "viewergrantleethemesupporttest.h"
using namespace Qt::Literals::StringLiterals;

#include "messageviewer/viewer.h"

#include <KActionCollection>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QStandardPaths>
#include <QTest>

ViewerGrantleeThemeSupportTest::ViewerGrantleeThemeSupportTest(QObject *parent)
    : QObject(parent)
{
    if (qEnvironmentVariableIntValue("KDECI_CANNOT_CREATE_WINDOWS")) {
        QSKIP("KDE CI can't create a window on this platform, skipping some gui tests");
    }
}

ViewerGrantleeThemeSupportTest::~ViewerGrantleeThemeSupportTest() = default;

void ViewerGrantleeThemeSupportTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);

    // Point the test to our dummy icon theme
    KConfigGroup cg(KSharedConfig::openConfig(), u"Icons"_s);
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
