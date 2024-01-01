/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "localdatabasemanagertest.h"
#include "../localdatabasemanager.h"
#include "../localdatabasemanager_p.h"

#include <QTest>

class TestLocalDatabaseManagerPrivate : public WebEngineViewer::LocalDataBaseManagerPrivate
{
public:
    TestLocalDatabaseManagerPrivate()
        : WebEngineViewer::LocalDataBaseManagerPrivate()
    {
    }

protected:
    void downloadDataBase(const QString &clientState) override
    {
        // don't actually download anything
    }
};

class TestLocalDataBaseManager : public WebEngineViewer::LocalDataBaseManager
{
public:
    TestLocalDataBaseManager(QObject *parent)
        : WebEngineViewer::LocalDataBaseManager(new TestLocalDatabaseManagerPrivate, parent)
    {
    }

    ~TestLocalDataBaseManager() override
    {
        delete d;
    }

    void setDownloadInfoSendByServer(const QString &data)
    {
        mDownloadInfoSendByServer = data;
    }

private:
    QString mDownloadInfoSendByServer;
};

LocalDataBaseManagerTest::LocalDataBaseManagerTest(QObject *parent)
    : QObject(parent)
{
}

LocalDataBaseManagerTest::~LocalDataBaseManagerTest() = default;

QTEST_GUILESS_MAIN(LocalDataBaseManagerTest)

#include "moc_localdatabasemanagertest.cpp"
