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

#include "localdatabasefile.h"
#include "webengineviewer_debug.h"
#include "downloadlocaldatabasethread.h"

#include <KConfigGroup>
#include <KConfig>

#include <QDir>
#include <QStandardPaths>
#include <QPointer>

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

namespace WebEngineViewer
{

class LocalDataBaseManagerPrivate
{
public:
    LocalDataBaseManagerPrivate()
        : mFile(databaseFullPath()),
          mSecondToStartRefreshing(0),
          mDataBaseOk(false),
          mDownloadProgress(false)
    {
        QDir().mkpath(localDataBasePath());
        readConfig();
    }
    virtual ~LocalDataBaseManagerPrivate()
    {
        if (downloadLocalDatabaseThread) {
            downloadLocalDatabaseThread->quit();
            downloadLocalDatabaseThread->wait();
            delete downloadLocalDatabaseThread;
        }
        saveConfig();
    }

    void initialize()
    {
        if (mDownloadProgress) {
            return;
        }
        if (!mDataBaseOk) {
            qCDebug(WEBENGINEVIEWER_LOG) << "Start to create database";
            if (!QFileInfo::exists(databaseFullPath())) {
                downloadDataBase(QString());
            } else {
                const uint now = QDateTime::currentDateTimeUtc().toTime_t();
                //qDebug() << " now "<< now << " d->mSecondToStartRefreshing "<<d->mSecondToStartRefreshing << " now > d->mSecondToStartRefreshing" << (now > d->mSecondToStartRefreshing);
                if ((mSecondToStartRefreshing != 0) && (mSecondToStartRefreshing > now)) {
                    qCWarning(WEBENGINEVIEWER_LOG) << " It's not necessary to check database now";
                    mDataBaseOk = true;
                } else {
                    //Perhaps don't download for each start of kmail
                    downloadDataBase(mNewClientState);
                }
            }
        }
    }

    virtual void downloadDataBase(const QString &clientState)
    {
        mDownloadProgress = true;
        downloadLocalDatabaseThread = new WebEngineViewer::DownloadLocalDatabaseThread;
        downloadLocalDatabaseThread->setDatabaseFullPath(databaseFullPath());
        downloadLocalDatabaseThread->setDataBaseState(clientState);
        QObject::connect(downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::createDataBaseFailed,
                         [this]() {
                            mDataBaseOk = false;
                            mDownloadProgress = false; 
                         });
        QObject::connect(downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::createDataBaseFinished,
                         [this](bool success, const QString &newClientState, const QString &minWaitDurationStr) {
                            mDataBaseOk = success;
                            mDownloadProgress = false;
                            mNewClientState = success ? newClientState : QString();
                            mMinimumWaitDuration = minWaitDurationStr;
                            saveConfig();
                            //if !success => redownload full!
                            if (!success) {
                                qCWarning(WEBENGINEVIEWER_LOG) << "We need to redownload full database";
                                downloadDataBase(QString()); // download full database
                            }
                         });
        QObject::connect(downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::finished,
                         downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::deleteLater);
        downloadLocalDatabaseThread->start();
    }

    void readConfig()
    {
        KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
        KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
        mNewClientState = grp.readEntry(QStringLiteral("DataBaseState"));
        mMinimumWaitDuration = grp.readEntry(QStringLiteral("RefreshDataBase"));
        if (!mMinimumWaitDuration.isEmpty()) {
            mSecondToStartRefreshing = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(mMinimumWaitDuration));
        }
    }

    void saveConfig()
    {
        KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
        KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
        grp.writeEntry(QStringLiteral("DataBaseState"), mNewClientState);
        grp.writeEntry(QStringLiteral("RefreshDataBase"), mMinimumWaitDuration);
    }

    LocalDataBaseFile mFile;
    QString mNewClientState;
    QString mMinimumWaitDuration;
    uint mSecondToStartRefreshing;
    bool mDataBaseOk;
    bool mDownloadProgress;
    QPointer<WebEngineViewer::DownloadLocalDatabaseThread> downloadLocalDatabaseThread;
};

}
