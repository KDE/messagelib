/*
   Copyright (c) 2015-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
