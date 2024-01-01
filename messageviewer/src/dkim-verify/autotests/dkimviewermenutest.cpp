/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimviewermenutest.h"
#include "dkim-verify/dkimviewermenu.h"
#include <QTest>
QTEST_MAIN(DKIMViewerMenuTest)
DKIMViewerMenuTest::DKIMViewerMenuTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMViewerMenuTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMViewerMenu menu;
    QVERIFY(menu.menu());
}

#include "moc_dkimviewermenutest.cpp"
