/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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
#include "localdatabasemanager.h"
#include "webengineviewer_debug.h"
#include "checkphishingurlfromlocaldatabasejob.h"

#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(LocalDataBaseManager, s_localDataBaseManager)

namespace {
inline QString tableName() {
    return QStringLiteral("malware");
}
}

LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : QObject(parent),
      mDataBaseOk(false)
{
}

LocalDataBaseManager::~LocalDataBaseManager()
{

}

void LocalDataBaseManager::start()
{
    mDataBaseOk = initializeDataBase();
    if (initializeDataBase()) {
        if ( !mDataBase.tables().contains( tableName() ) ) {
            createTable();
        }
    }
}

QString LocalDataBaseManager::localDataBasePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl/");
}

LocalDataBaseManager *LocalDataBaseManager::self()
{
    return s_localDataBaseManager;
}

bool LocalDataBaseManager::initializeDataBase()
{
    const QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open DataBase: " << err.text();
        return false;
    }
    return true;
}

bool LocalDataBaseManager::createTable()
{
    QSqlQuery query;
    query.exec(QStringLiteral("create table %1 (id int primary key, "
                              "hash varchar(20))").arg(tableName()));
    return true;
}

QSqlError LocalDataBaseManager::initDb()
{
    mDataBase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    qDebug()<<" localDataBasePath()"<<localDataBasePath();
    QDir().mkpath(localDataBasePath());
    mDataBase.setDatabaseName(localDataBasePath() + QStringLiteral("/malwaredb.sql"));
    if (!mDataBase.open()) {
        return mDataBase.lastError();
    }
    return QSqlError();
}

void LocalDataBaseManager::checkUrl(const QUrl &url)
{
    if (mDataBaseOk) {

    } else {
        Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::Unknown);
    }
    //TODO
}

