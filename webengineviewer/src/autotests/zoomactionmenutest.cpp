/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "zoomactionmenutest.h"
#include "../widgets/zoomactionmenu.h"
#include <QTest>
#include <KActionCollection>

ZoomActionMenuTest::ZoomActionMenuTest(QObject *parent)
    : QObject(parent)
{
}

ZoomActionMenuTest::~ZoomActionMenuTest()
{
}

void ZoomActionMenuTest::shouldHaveDefaultValue()
{
    WebEngineViewer::ZoomActionMenu menu(this);
    menu.setActionCollection(new KActionCollection(this));
    menu.createZoomActions();

    QVERIFY(menu.zoomInAction());
    QVERIFY(menu.zoomOutAction());
    QVERIFY(menu.zoomResetAction());
}

void ZoomActionMenuTest::shouldAssignZoomFactor()
{
    WebEngineViewer::ZoomActionMenu menu(this);
    menu.setActionCollection(new KActionCollection(this));
    menu.createZoomActions();
    const qreal initialValue = 50;
    menu.setZoomFactor(initialValue);
    QCOMPARE(menu.zoomFactor(), initialValue);
}

QTEST_MAIN(ZoomActionMenuTest)
