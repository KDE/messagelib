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
    LocalDataBaseManagerPrivate()
        : mDataBaseOk(false),
          mDownloadProgress(false)
    {
        QDir().mkpath(localDataBasePath());
        readConfig();
    }
    ~LocalDataBaseManagerPrivate()
    {
        saveConfig();
    }

    void readConfig();
    void saveConfig();
    QString mNewClientState;
    bool mDataBaseOk;
    bool mDownloadProgress;
};

LocalDataBaseManager::LocalDataBaseManager(QObject *parent)
    : QObject(parent),
      d(new LocalDataBaseManagerPrivate)

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
    qDebug() << " void LocalDataBaseManager::downloadDataBase(const QString &clientState)"<<clientState;
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
        qCWarning(WEBENGINEVIEWER_LOG) << "Database already initialized.";
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
            qDebug() << "No update necessary ";
        } else {
            switch (infoDataBase.responseType) {
            case WebEngineViewer::UpdateDataBaseInfo::FullUpdate:
            case WebEngineViewer::UpdateDataBaseInfo::PartialUpdate:
                installNewDataBase(infoDataBase);
                break;
            case WebEngineViewer::UpdateDataBaseInfo::Unknown:
                break;
            }
        }
    }
    d->mDownloadProgress = false;
}


void LocalDataBaseManager::installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    WebEngineViewer::CreateDatabaseFileJob *job = new WebEngineViewer::CreateDatabaseFileJob(this);
    job->setFileName(databaseFullPath());
    job->setUpdateDataBaseInfo(infoDataBase);
    connect(job, &CreateDatabaseFileJob::finished, this, &LocalDataBaseManager::slotCreateDataBaseFileNameFinished);
    job->start();
}

void LocalDataBaseManager::slotCreateDataBaseFileNameFinished(bool finished, const QString &newClientState)
{
    d->mDownloadProgress = false;
    d->mNewClientState = finished ? newClientState : QString();
    d->saveConfig();
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
