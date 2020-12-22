/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentinfotest.h"
#include "remote-content/remotecontentinfo.h"
#include <QTest>
QTEST_MAIN(RemoteContentInfoTest)
RemoteContentInfoTest::RemoteContentInfoTest(QObject *parent)
    : QObject(parent)
{

}

void RemoteContentInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentInfo info;
    //TODO
}
