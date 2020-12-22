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
    QVERIFY(info.url().isEmpty());
    QCOMPARE(info.status(), MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Unknown);
}
