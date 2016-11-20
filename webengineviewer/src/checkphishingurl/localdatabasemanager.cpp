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
#include "createphishingurldatabasejob.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QCryptographicHash>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(LocalDataBaseManager, s_localDataBaseManager)

namespace {
inline QString localDataBasePath() {
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl/");
}
}

LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : QObject(parent),
      mDataBaseOk(false),
      mDownloadProgress(false)
{
    readConfig();
}

LocalDataBaseManager::~LocalDataBaseManager()
{
}

void LocalDataBaseManager::closeDataBaseAndDeleteIt()
{
    /*
    if (mDataBaseOk) {
        mDataBase.close();
        QFile f(localDataBasePath() + sqlFileName());
        if (!f.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "impossible to remove local database file";
        }
    }
    */
}

void LocalDataBaseManager::readConfig()
{
    KConfig phishingurlKConfig(QStringLiteral("phishingurlrc"));
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    mNewClientState = grp.readEntry(QStringLiteral("DataBaseState"));
}

void LocalDataBaseManager::saveConfig()
{
    KConfig phishingurlKConfig(QStringLiteral("phishingurlrc"));
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    grp.writeEntry(QStringLiteral("DataBaseState"), mNewClientState);
}

void LocalDataBaseManager::downloadPartialDataBase()
{
    setDownloadProgress(true);
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(mNewClientState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &LocalDataBaseManager::slotDownloadDataBaseFinished);
    job->start();
}

void LocalDataBaseManager::downloadFullDataBase()
{
    setDownloadProgress(true);
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &LocalDataBaseManager::slotDownloadDataBaseFinished);
    job->start();
}

void LocalDataBaseManager::initialize()
{
    if (mDownloadProgress) {
        return;
    }
    if (!mDataBaseOk) {
        /*
        bool initDatabaseSuccess = initializeDataBase();
        if (initDatabaseSuccess) {
            if (!mDataBase.tables().contains(tableName())) {
                if (createTable()) {
                    downloadFullDataBase();
                } else {
                    qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to create Table";
                }
            } else {
                mDataBaseOk = true;
            }
        }
        */
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database already initialized.";
    }
    if (mDataBaseOk) {
    }
}

void LocalDataBaseManager::slotCheckDataBase()
{
    if (mDataBaseOk && !mDownloadProgress) {
        downloadPartialDataBase();
    }
}

void LocalDataBaseManager::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
                                                            WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{

    qDebug() << "LocalDataBaseManager::slotDownloadFullDataBaseFinished "<<status;
    switch(status) {
    case CreatePhishingUrlDataBaseJob::InvalidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Invalid Data.";
        mDataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::ValidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Valid Data.";
        mDataBaseOk = true;
        break;
    case CreatePhishingUrlDataBaseJob::UnknownError:
        qCWarning(WEBENGINEVIEWER_LOG) << "Unknown data.";
        mDataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::BrokenNetwork:
        qCWarning(WEBENGINEVIEWER_LOG) << "Broken Networks.";
        mDataBaseOk = false;
        break;
    }
    if (mDataBaseOk) {
        if ((infoDataBase.responseType == WebEngineViewer::UpdateDataBaseInfo::PartialUpdate) &&
                (mNewClientState == infoDataBase.newClientState)) {
            qDebug() << "No update necessary ";
        } else {
            //qDebug() << "infoDataBase" << infoDataBase.additionList.count();
            switch(infoDataBase.responseType) {
            case WebEngineViewer::UpdateDataBaseInfo::FullUpdate:
                fullUpdateDataBase(infoDataBase);
                break;
            case WebEngineViewer::UpdateDataBaseInfo::PartialUpdate:
                partialUpdateDataBase(infoDataBase);
                break;
            case WebEngineViewer::UpdateDataBaseInfo::Unknown:
                //Signal it ?
                return;
                break;
            }
        }
    }
    checkDataBase();
    mDownloadProgress = false;
}

void LocalDataBaseManager::checkDataBase()
{
    //TODO get all hash and use sha256
    //TODO
}

void LocalDataBaseManager::removeElementFromDataBase(const QVector<Removal> &removalList)
{
    Q_FOREACH(const Removal &removeItem, removalList) {
        Q_FOREACH (int id, removeItem.indexes) {
            //TODO
        }
    }
}

void LocalDataBaseManager::addElementToDataBase(const QVector<Addition> &additionList)
{
    Q_FOREACH(const Addition &add, additionList) {
        //qDebug() << " add.size" << add.prefixSize;
        //qDebug() << " add.hash" << QByteArray::fromBase64(add.hashString).size();
        const QByteArray uncompressed = QByteArray::fromBase64(add.hashString);
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;
            //qDebug() << "m " << m << " m.size" << m.size();
            //TODO add in database
        }
    }
}

void LocalDataBaseManager::fullUpdateDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    //Clear DataBase
    addElementToDataBase(infoDataBase.additionList);
    mNewClientState = infoDataBase.newClientState;
    saveConfig();
}

void LocalDataBaseManager::partialUpdateDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    removeElementFromDataBase(infoDataBase.removalList);
    addElementToDataBase(infoDataBase.additionList);
    mNewClientState = infoDataBase.newClientState;
    saveConfig();
}

LocalDataBaseManager *LocalDataBaseManager::self()
{
    return s_localDataBaseManager;
}

bool LocalDataBaseManager::initializeDataBase()
{
    /*
    mDataBase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    QDir().mkpath(localDataBasePath());
    mDataBase.setDatabaseName(localDataBasePath() + sqlFileName());
    if (!mDataBase.open()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open DataBase: " << mDataBase.lastError().text();
        return false;
    }
    */
    return true;
}

bool LocalDataBaseManager::createTable()
{
    return true;
    /*
    QSqlQuery query(mDataBase);
    return query.exec(QStringLiteral("create table %1 (id int primary key, "
                              "hash varchar(32))").arg(tableName()));
                              */
}

void LocalDataBaseManager::setDownloadProgress(bool downloadProgress)
{
    mDownloadProgress = downloadProgress;
}

void LocalDataBaseManager::checkUrl(const QUrl &url)
{
    if (mDataBaseOk) {
        QByteArray hash = createHash(url);
        if (malwareFound(hash)) {
            Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::Malware);
        } else {
            Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::UrlOk);
        }
    } else {
        Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::Unknown);
    }
}

QByteArray LocalDataBaseManager::createHash(const QUrl &url)
{
    return QCryptographicHash::hash(QByteArray() /*TODO use url*/, QCryptographicHash::Sha256);
}

bool LocalDataBaseManager::malwareFound(const QByteArray &hash)
{
    //TODO
    return false;
}
