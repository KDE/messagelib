/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "downloadlocaldatabasethread.h"
#include "localdatabasefile.h"
#include "webengineviewer_debug.h"

#include <KConfig>
#include <KConfigGroup>

#include <QDir>
#include <QPointer>
#include <QStandardPaths>

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
        : mFile(databaseFullPath())
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
                const uint now = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
                // qDebug() << " now "<< now << " d->mSecondToStartRefreshing "<<d->mSecondToStartRefreshing << " now > d->mSecondToStartRefreshing" << (now >
                // d->mSecondToStartRefreshing);
                if ((mSecondToStartRefreshing != 0) && (mSecondToStartRefreshing > now)) {
                    qCDebug(WEBENGINEVIEWER_LOG) << " It's not necessary to check database now";
                    mDataBaseOk = true;
                } else {
                    // Perhaps don't download for each start of kmail
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
        QObject::connect(downloadLocalDatabaseThread.data(), &DownloadLocalDatabaseThread::createDataBaseFailed, downloadLocalDatabaseThread.data(), [this]() {
            mDataBaseOk = false;
            mDownloadProgress = false;
        });
        QObject::connect(downloadLocalDatabaseThread.data(),
                         &DownloadLocalDatabaseThread::createDataBaseFinished,
                         downloadLocalDatabaseThread.data(),
                         [this](bool success, const QString &newClientState, const QString &minWaitDurationStr) {
                             mDataBaseOk = success;
                             mDownloadProgress = false;
                             mNewClientState = success ? newClientState : QString();
                             mMinimumWaitDuration = minWaitDurationStr;
                             saveConfig();
                             // if !success => redownload full!
                             if (!success) {
                                 qCWarning(WEBENGINEVIEWER_LOG) << "We need to redownload full database";
                                 downloadDataBase(QString()); // download full database
                             }
                         });
        QObject::connect(downloadLocalDatabaseThread.data(),
                         &DownloadLocalDatabaseThread::finished,
                         downloadLocalDatabaseThread.data(),
                         &DownloadLocalDatabaseThread::deleteLater);
        downloadLocalDatabaseThread->start();
    }

    void readConfig()
    {
        KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
        KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
        mNewClientState = grp.readEntry(QStringLiteral("DataBaseState"));
        mMinimumWaitDuration = grp.readEntry(QStringLiteral("RefreshDataBase"));
        if (!mMinimumWaitDuration.isEmpty()) {
            mSecondToStartRefreshing = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(
                WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(mMinimumWaitDuration));
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
    uint mSecondToStartRefreshing = 0;
    bool mDataBaseOk = false;
    bool mDownloadProgress = false;
    QPointer<WebEngineViewer::DownloadLocalDatabaseThread> downloadLocalDatabaseThread;
};
}
