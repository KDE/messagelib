/*
   SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmanagertest.h"
using namespace Qt::Literals::StringLiterals;

#include "remote-content/remotecontentinfo.h"
#include <QStandardPaths>
#include <QTest>
QTEST_MAIN(RemoteContentManagerTest)
RemoteContentManagerTest::RemoteContentManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

RemoteContentManagerTest::~RemoteContentManagerTest()
{
    mRemoveContentManager.clear();
}

void RemoteContentManagerTest::shouldHaveDefaultValues()
{
    QVERIFY(mRemoveContentManager.removeContentInfo().isEmpty());
}

void RemoteContentManagerTest::shouldIsBlocked_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("blocked");
    QTest::addColumn<bool>("contains");
    QTest::newRow("empty") << QUrl() << false << false;
    MessageViewer::RemoteContentInfo info;
    info.setUrl(u"http://www.kde.org"_s);
    info.setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Blocked);

    mRemoveContentManager.addRemoteContent(info);
    QTest::newRow("kde-blocked") << QUrl(u"http://www.kde.org"_s) << false << true;
    MessageViewer::RemoteContentInfo info2;
    info2.setUrl(u"http://www.kde2.org"_s);
    info2.setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus::Authorized);
    mRemoveContentManager.addRemoteContent(info2);

    QTest::newRow("kde-authorized") << QUrl(u"http://www.kde2.org"_s) << true << true;
}

void RemoteContentManagerTest::shouldIsBlocked()
{
    QFETCH(QUrl, url);
    QFETCH(bool, blocked);
    QFETCH(bool, contains);
    bool isInList = false;
    const bool result = mRemoveContentManager.isAutorized(url, isInList);
    QCOMPARE(isInList, contains);
    QCOMPARE(blocked, result);
}

#include "moc_remotecontentmanagertest.cpp"
