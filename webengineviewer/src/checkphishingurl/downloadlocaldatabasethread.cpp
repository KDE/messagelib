/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "downloadlocaldatabasethread.h"
#include "createdatabasefilejob.h"
#include "webengineviewer_debug.h"
using namespace WebEngineViewer;

DownloadLocalDatabaseThread::DownloadLocalDatabaseThread(QObject *parent)
    : QThread(parent)
{
    qCDebug(WEBENGINEVIEWER_LOG) << "DownloadLocalDatabaseThread::DownloadLocalDatabaseThread " << this;
}

DownloadLocalDatabaseThread::~DownloadLocalDatabaseThread()
{
    qCDebug(WEBENGINEVIEWER_LOG) << "DownloadLocalDatabaseThread::~DownloadLocalDatabaseThread " << this;
}

void DownloadLocalDatabaseThread::setDataBaseState(const QString &value)
{
    mCurrentDataBaseState = value;
}

void DownloadLocalDatabaseThread::run()
{
    if (mDatabaseFullPath.isEmpty()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Database full path is empty";
        Q_EMIT createDataBaseFailed();
        deleteLater();
        return;
    }
    auto job = new WebEngineViewer::CreatePhishingUrlDataBaseJob;
    job->moveToThread(this);
    job->setDataBaseDownloadNeeded(mCurrentDataBaseState.isEmpty() ? WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase
                                                                   : WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(mCurrentDataBaseState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &DownloadLocalDatabaseThread::slotDownloadDataBaseFinished);
    job->start();
    exec();
}

void DownloadLocalDatabaseThread::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
                                                               WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{
    bool dataBaseOk = false;
    switch (status) {
    case CreatePhishingUrlDataBaseJob::InvalidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Invalid Data.";
        dataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::ValidData:
        qCDebug(WEBENGINEVIEWER_LOG) << "Valid Data.";
        dataBaseOk = true;
        break;
    case CreatePhishingUrlDataBaseJob::UnknownError:
        qCWarning(WEBENGINEVIEWER_LOG) << "Unknown data.";
        dataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::BrokenNetwork:
        qCWarning(WEBENGINEVIEWER_LOG) << "Broken Networks.";
        dataBaseOk = false;
        break;
    }
    if (dataBaseOk) {
        if (mCurrentDataBaseState == infoDataBase.newClientState) {
            qCDebug(WEBENGINEVIEWER_LOG) << "No update necessary ";
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
    } else {
        Q_EMIT createDataBaseFailed();
        quit();
    }
}

void DownloadLocalDatabaseThread::setDatabaseFullPath(const QString &databaseFullPath)
{
    mDatabaseFullPath = databaseFullPath;
}

void DownloadLocalDatabaseThread::installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    auto job = new WebEngineViewer::CreateDatabaseFileJob;
    job->setFileName(mDatabaseFullPath);
    job->setUpdateDataBaseInfo(infoDataBase);
    connect(job, &CreateDatabaseFileJob::finished, this, &DownloadLocalDatabaseThread::slotCreateDataBaseFileNameFinished);
    job->start();
}

void DownloadLocalDatabaseThread::slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr)
{
    Q_EMIT createDataBaseFinished(success, newClientState, minimumWaitDurationStr);
    quit();
}

#include "moc_downloadlocaldatabasethread.cpp"
