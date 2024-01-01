/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentinfotest.h"
#include "remote-content/remotecontentinfo.h"
#include <QTest>
QTEST_GUILESS_MAIN(RemoteContentInfoTest)
RemoteContentInfoTest::RemoteContentInfoTest(QObject *parent)
    : QObject(parent)
{
}

void RemoteContentInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentInfo info;
    QVERIFY(info.url().isEmpty());
    QCOMPARE(info.status(), MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Unknown);
    QVERIFY(!info.isValid());
}

void RemoteContentInfoTest::shouldVerifyIsValid()
{
    MessageViewer::RemoteContentInfo info;
    QCOMPARE(info.status(), MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Unknown);
    QVERIFY(!info.isValid());
    info.setUrl(QStringLiteral("bla"));
    QVERIFY(!info.isValid());
    info.setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Blocked);
    QVERIFY(info.isValid());
    info.setUrl({});
    QVERIFY(!info.isValid());
}

#include "moc_remotecontentinfotest.cpp"
