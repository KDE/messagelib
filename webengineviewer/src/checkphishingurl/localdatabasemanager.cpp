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
#include "createdatabasefilejob.h"
#include "checkphishingurlutil.h"
#include "localdatabasefile.h"
#include "urlhashing.h"

#include <KConfigGroup>
#include <KSharedConfig>

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
          mDataBaseOk(false),
          mDownloadProgress(false),
          q(qq)
    {
        QDir().mkpath(localDataBasePath());
        readConfig();
    }
    ~LocalDataBaseManagerPrivate()
    {
        saveConfig();
    }

    void installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);
    void readConfig();
    void saveConfig();
    LocalDataBaseFile mFile;
    QString mNewClientState;
    QString mMinimumWaitDuration;
    bool mDataBaseOk;
    bool mDownloadProgress;
    LocalDataBaseManager *q;
};

void LocalDataBaseManagerPrivate::installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    WebEngineViewer::CreateDatabaseFileJob *job = new WebEngineViewer::CreateDatabaseFileJob(q);
    job->setFileName(databaseFullPath());
    job->setUpdateDataBaseInfo(infoDataBase);
    q->connect(job, &CreateDatabaseFileJob::finished, q, &LocalDataBaseManager::slotCreateDataBaseFileNameFinished);
    job->start();
}


LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : QObject(parent),
      d(new LocalDataBaseManagerPrivate(this))

{
    initialize();
}

LocalDataBaseManager::~LocalDataBaseManager()
{
    delete d;
}

void LocalDataBaseManagerPrivate::readConfig()
{
    KConfig phishingurlKConfig(QStringLiteral("phishingurlrc"));
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    mNewClientState = grp.readEntry(QStringLiteral("DataBaseState"));
}

void LocalDataBaseManagerPrivate::saveConfig()
{
    KConfig phishingurlKConfig(QStringLiteral("phishingurlrc"));
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("General"));
    grp.writeEntry(QStringLiteral("DataBaseState"), mNewClientState);
}

void LocalDataBaseManager::downloadDataBase(const QString &clientState)
{
    setDownloadProgress(true);
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setDataBaseDownloadNeeded(clientState.isEmpty() ? WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase : WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(clientState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &LocalDataBaseManager::slotDownloadDataBaseFinished);
    job->start();
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
        if (!QFile(databaseFullPath()).exists()) {
            downloadFullDataBase();
        } else {
            //Perhaps don't download for each start of kmail
            downloadPartialDataBase();
        }
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database already initialized. It's a bug in code if you call it twice.";
    }
}

void LocalDataBaseManager::slotCheckDataBase()
{
    if (d->mDataBaseOk && !d->mDownloadProgress) {
        downloadPartialDataBase();
    }
}

void LocalDataBaseManager::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
        WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{
    switch (status) {
    case CreatePhishingUrlDataBaseJob::InvalidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Invalid Data.";
        d->mDataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::ValidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Valid Data.";
        d->mDataBaseOk = true;
        break;
    case CreatePhishingUrlDataBaseJob::UnknownError:
        qCWarning(WEBENGINEVIEWER_LOG) << "Unknown data.";
        d->mDataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::BrokenNetwork:
        qCWarning(WEBENGINEVIEWER_LOG) << "Broken Networks.";
        d->mDataBaseOk = false;
        break;
    }
    if (d->mDataBaseOk) {
        if (d->mNewClientState == infoDataBase.newClientState) {
            qCDebug(WEBENGINEVIEWER_LOG) << "No update necessary ";
        } else {
            switch (infoDataBase.responseType) {
            case WebEngineViewer::UpdateDataBaseInfo::FullUpdate:
            case WebEngineViewer::UpdateDataBaseInfo::PartialUpdate:
                d->installNewDataBase(infoDataBase);
                break;
            case WebEngineViewer::UpdateDataBaseInfo::Unknown:
                break;
            }
        }
    }
    d->mDownloadProgress = false;
}



void LocalDataBaseManager::slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState)
{
    d->mDownloadProgress = false;
    d->mNewClientState = success ? newClientState : QString();
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
        QByteArray hash;
        QByteArray result = d->mFile.searchHash(hash);
        if (hash.contains(result)) {
            if (d->mNewClientState.isEmpty()) {
                qCWarning(WEBENGINEVIEWER_LOG) << "Database client state is unknown";
                Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::Unknown);
            } else {
                WebEngineViewer::SearchFullHashJob *job = new WebEngineViewer::SearchFullHashJob(this);
                job->setDatabaseState(QStringList() << d->mNewClientState);
                connect(job, &SearchFullHashJob::result, this, &LocalDataBaseManager::slotSearchOnServerResult);
                job->start();
            }
        } else {
            Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::UrlOk);
        }
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database not ok";
        Q_EMIT checkUrlFinished(url, WebEngineViewer::LocalDataBaseManager::Unknown);
    }
    if (d->mFile.checkFileChanged()) {
        d->mFile.reload();
    }
}

void LocalDataBaseManager::slotSearchOnServerResult(WebEngineViewer::SearchFullHashJob::UrlStatus status, const QByteArray &hash, const QStringList &listHash)
{
    qCDebug(WEBENGINEVIEWER_LOG) << "hash " << hash << " listHash " << listHash;
    //TODO
}
