/*
   Copyright (C) 2014-2016 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "globalparttest.h"
#include <qtest.h>
#include <../src/part/globalpart.h>
GlobalPartTest::GlobalPartTest(QObject *parent)
    : QObject(parent)
{

}

GlobalPartTest::~GlobalPartTest()
{

}

void GlobalPartTest::shouldHaveDefaultValue()
{
    MessageComposer::GlobalPart globalpart;
    QVERIFY(globalpart.isGuiEnabled());
    QVERIFY(!globalpart.parentWidgetForGui());
    QVERIFY(!globalpart.isFallbackCharsetEnabled());
    QVERIFY(!globalpart.is8BitAllowed());
    QVERIFY(!globalpart.MDNRequested());
}

QTEST_MAIN(GlobalPartTest)
