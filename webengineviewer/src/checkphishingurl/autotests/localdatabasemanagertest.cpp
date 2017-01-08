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

#include <QTest>

class TestLocalDataBaseManager : public WebEngineViewer::LocalDataBaseManager
{
public:
    TestLocalDataBaseManager(QObject *parent)
        : WebEngineViewer::LocalDataBaseManager(parent)
    {

    }

    void setDownloadInfoSendByServer(const QString &data)
    {
        mDownloadInfoSendByServer = data;
    }

    // LocalDataBaseManager interface
protected:
    void downloadFullDataBase() Q_DECL_OVERRIDE;
    void downloadPartialDataBase() Q_DECL_OVERRIDE;
private:
    QString mDownloadInfoSendByServer;
};

void TestLocalDataBaseManager::downloadFullDataBase()
{
}

void TestLocalDataBaseManager::downloadPartialDataBase()
{
}

LocalDataBaseManagerTest::LocalDataBaseManagerTest(QObject *parent)
    : QObject(parent)
{

}

LocalDataBaseManagerTest::~LocalDataBaseManagerTest()
{

}

QTEST_MAIN(LocalDataBaseManagerTest)
