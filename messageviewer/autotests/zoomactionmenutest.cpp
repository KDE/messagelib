/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "zoomactionmenutest.h"
#include "../src/widgets/zoomactionmenu.h"
#include <KActionCollection>
#include <QTest>

ZoomActionMenuTest::ZoomActionMenuTest(QObject *parent)
    : QObject(parent)
{
}

ZoomActionMenuTest::~ZoomActionMenuTest()
{
}

void ZoomActionMenuTest::shouldHaveDefaultValue()
{
    MessageViewer::ZoomActionMenu menu(this);
    menu.setActionCollection(new KActionCollection(this));
    menu.createZoomActions();

    QVERIFY(menu.zoomInAction());
    QVERIFY(menu.zoomOutAction());
    QVERIFY(menu.zoomResetAction());
}

void ZoomActionMenuTest::shouldAssignZoomFactor()
{
    MessageViewer::ZoomActionMenu menu(this);
    menu.setActionCollection(new KActionCollection(this));
    menu.createZoomActions();
    qreal initialValue = 50;
    menu.setZoomFactor(initialValue);
    QCOMPARE(menu.zoomFactor(), initialValue);
}

QTEST_MAIN(ZoomActionMenuTest)

#include "moc_zoomactionmenutest.cpp"
