/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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

    ~TestLocalDataBaseManager()
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

LocalDataBaseManagerTest::~LocalDataBaseManagerTest()
{

}

QTEST_MAIN(LocalDataBaseManagerTest)
