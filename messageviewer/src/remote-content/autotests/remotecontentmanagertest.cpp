/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmanagertest.h"
#include "remote-content/remotecontentmanager.h"
#include "remote-content/remotecontentinfo.h"
#include <QStandardPaths>
#include <QTest>
QTEST_MAIN(RemoteContentManagerTest)
RemoteContentManagerTest::RemoteContentManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void RemoteContentManagerTest::shouldHaveDefaultValues()
{
    MessageViewer::RemoteContentManager m;
    QVERIFY(m.removeContentInfo().isEmpty());
}

void RemoteContentManagerTest::shouldIsBlocked_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("blocked");
    QTest::addColumn<bool>("contains");
    QTest::newRow("empty") << QUrl() << false << false;
    //QTest::newRow("empty") << QUrl() << false << false;
    //TODO add elment + clear;

}

void RemoteContentManagerTest::shouldIsBlocked()
{
    QFETCH(QUrl, url);
    QFETCH(bool, blocked);
    QFETCH(bool, contains);
    MessageViewer::RemoteContentManager m;
    bool isInList = false;
    const bool result = m.isAutorized(url, isInList);
    QCOMPARE(isInList, contains);
    QCOMPARE(blocked, result);
}
