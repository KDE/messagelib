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
#include "localdatabasemanager.h"
#include "webengineviewer_debug.h"
#include "createphishingurldatabasejob.h"
#include "createdatabasefilejob.h"
#include "checkphishingurlutil.h"
#include "localdatabasefile.h"
#include "downloadlocaldatabasethread.h"
#include "urlhashing.h"
#include "backoffmodemanager.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QPointer>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QCryptographicHash>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(LocalDataBaseManager, s_localDataBaseManager)

namespace
{
inline QString localDataBasePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl/");
}
inline QString databaseFullPath()
{
    return localDataBasePath() + QLatin1Char('/') + WebEngineViewer::CheckPhishingUrlUtil::databaseFileName();
}
}

class WebEngineViewer::LocalDataBaseManagerPrivate
{
public:
    LocalDataBaseManagerPrivate(LocalDataBaseManager *qq)
        : mFile(databaseFullPath()),
          mSecondToStartRefreshing(0),
          mDataBaseOk(false),
          mDownloadProgress(false),
          q(qq)
    {
        QDir().mkpath(localDataBasePath());
        readConfig();
    }
    ~LocalDataBaseManagerPrivate()
    {
        if (downloadLocalDatabaseThread) {
            downloadLocalDatabaseThread->quit();
            downloadLocalDatabaseThread->wait();
            delete downloadLocalDatabaseThread;
        }
        saveConfig();
    }

    void readConfig();
    void saveConfig();
    LocalDataBaseFile mFile;
    QString mNewClientState;
    QString mMinimumWaitDuration;
    uint mSecondToStartRefreshing;
    bool mDataBaseOk;
    bool mDownloadProgress;
    QPointer<WebEngineViewer::DownloadLocalDatabaseThread> downloadLocalDatabaseThread;
    LocalDataBaseManager *q;
};

LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : QObject(parent),
      d(new LocalDataBaseManagerPrivate(this))

{
    qRegisterMetaType<WebEngineViewer::UpdateDataBaseInfo>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType>();
}

LocalDataBaseManager::~LocalDataBaseManager()
{
    delete d;
}

void LocalDataBaseManagerPrivate::readConfig()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    mNewClientState = grp.readEntry(QStringLiteral("DataBaseState"));
    mMinimumWaitDuration = grp.readEntry(QStringLiteral("RefreshDataBase"));
    if (!mMinimumWaitDuration.isEmpty()) {
        mSecondToStartRefreshing = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(mMinimumWaitDuration));
    }
}

void LocalDataBaseManagerPrivate::saveConfig()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    grp.writeEntry(QStringLiteral("DataBaseState"), mNewClientState);
    grp.writeEntry(QStringLiteral("RefreshDataBase"), mMinimumWaitDuration);
}

void LocalDataBaseManager::downloadDataBase(const QString &clientState)
{
    setDownloadProgress(true);
    d->downloadLocalDatabaseThread = new WebEngineViewer::DownloadLocalDatabaseThread;
    d->downloadLocalDatabaseThread->setDatabaseFullPath(databaseFullPath());
    d->downloadLocalDatabaseThread->setDataBaseState(clientState);
    connect(d->downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::createDataBaseFailed, this, &LocalDataBaseManager::slotCreateDataBaseFailed);
    connect(d->downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::createDataBaseFinished, this, &LocalDataBaseManager::slotCreateDataBaseFileNameFinished);
    connect(d->downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::finished, d->downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::deleteLater);
    d->downloadLocalDatabaseThread->start();
}

void LocalDataBaseManager::slotCreateDataBaseFailed()
{
    d->mDataBaseOk = false;
    d->mDownloadProgress = false;
}

void LocalDataBaseManager::downloadPartialDataBase()
{
    downloadDataBase(d->mNewClientState);
}

void LocalDataBaseManager::downloadFullDataBase()
{
    downloadDataBase(QString());
}

void LocalDataBaseManager::initialize()
{
    if (d->mDownloadProgress) {
        return;
    }
    if (!d->mDataBaseOk) {
        qCDebug(WEBENGINEVIEWER_LOG) << "Start to create database";
        if (!QFile(databaseFullPath()).exists()) {
            downloadFullDataBase();
        } else {
            const uint now = QDateTime::currentDateTime().toTime_t();
            //qDebug() << " now "<< now << " d->mSecondToStartRefreshing "<<d->mSecondToStartRefreshing << " now > d->mSecondToStartRefreshing" << (now > d->mSecondToStartRefreshing);
            if ((d->mSecondToStartRefreshing != 0) && (d->mSecondToStartRefreshing > now)) {
                qCWarning(WEBENGINEVIEWER_LOG) << " It's not necessary to check database now";
                d->mDataBaseOk = true;
            } else {
                //Perhaps don't download for each start of kmail
                downloadPartialDataBase();
            }
        }
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database already initialized. It's a bug in code if you call it twice.";
    }
}

void LocalDataBaseManager::slotCheckDataBase()
{
    const uint now = QDateTime::currentDateTime().toTime_t();
    if (d->mDataBaseOk && !d->mDownloadProgress && (d->mSecondToStartRefreshing < now)) {
        downloadPartialDataBase();
    }
}

void LocalDataBaseManager::slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr)
{
    d->mDataBaseOk = success;
    d->mDownloadProgress = false;
    d->mNewClientState = success ? newClientState : QString();
    d->mMinimumWaitDuration = minimumWaitDurationStr;
    d->saveConfig();
    //if !success => redownload full!
    if (!success) {
        qCWarning(WEBENGINEVIEWER_LOG) << "We need to redownload full database";
        downloadFullDataBase();
    }
}

LocalDataBaseManager *LocalDataBaseManager::self()
{
    return s_localDataBaseManager;
}

void LocalDataBaseManager::setDownloadProgress(bool downloadProgress)
{
    d->mDownloadProgress = downloadProgress;
}

void LocalDataBaseManager::checkUrl(const QUrl &url)
{
    if (d->mDataBaseOk) {
        //TODO fixme short hash! we don't need to use it.
        WebEngineViewer::UrlHashing urlHashing(url);
        QHash<QByteArray, QByteArray> hashList = urlHashing.hashList();
        QHash<QByteArray, QByteArray> conflictHashs;

        QHashIterator<QByteArray, QByteArray> i(hashList);
        while (i.hasNext()) {
            i.next();
            const QByteArray ba = i.value();
            QByteArray result = d->mFile.searchHash(ba);
            if (ba.contains(result)) {
                conflictHashs.insert(i.key().toBase64(), ba.toBase64());
            }
        }
        if (conflictHashs.isEmpty()) {
            Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Ok);
        } else {
            qCWarning(WEBENGINEVIEWER_LOG) << " We need to Check Server Database";
            if (d->mNewClientState.isEmpty()) {
                qCWarning(WEBENGINEVIEWER_LOG) << "Database client state is unknown";
                Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Unknown);
            } else {
                WebEngineViewer::SearchFullHashJob *job = new WebEngineViewer::SearchFullHashJob(this);
                job->setDatabaseState(QStringList() << d->mNewClientState);
                job->setSearchHashs(conflictHashs);
                job->setSearchFullHashForUrl(url);
                connect(job, &SearchFullHashJob::result, this, &LocalDataBaseManager::slotSearchOnServerResult);
                job->start();
            }
        }
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database not ok";
        Q_EMIT checkUrlFinished(url, WebEngineViewer::CheckPhishingUrlUtil::Unknown);
    }
    if (d->mFile.checkFileChanged()) {
        d->mFile.reload();
    }
}

void LocalDataBaseManager::slotSearchOnServerResult(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url)
{
    qCWarning(WEBENGINEVIEWER_LOG) << " Url " << url << " status " << status;
    Q_EMIT checkUrlFinished(url, status);
}
