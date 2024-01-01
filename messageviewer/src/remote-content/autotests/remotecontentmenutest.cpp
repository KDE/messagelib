/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenutest.h"
#include "remote-content/remotecontentmenu.h"
#include <QTest>

QTEST_MAIN(RemoteContentMenuTest)
RemoteContentMenuTest::RemoteContentMenuTest(QObject *parent)
    : QObject(parent)
{
}

void RemoteContentMenuTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentMenu m;
    QVERIFY(m.urls().isEmpty());
}

#include "moc_remotecontentmenutest.cpp"
