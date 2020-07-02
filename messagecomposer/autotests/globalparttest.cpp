/*
   SPDX-FileCopyrightText: 2014-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "globalparttest.h"
#include <QTest>
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
    QVERIFY(!globalpart.requestDeleveryConfirmation());
}

QTEST_MAIN(GlobalPartTest)
