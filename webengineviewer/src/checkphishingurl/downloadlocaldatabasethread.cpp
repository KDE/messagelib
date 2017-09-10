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

#include "createdatabasefilejob.h"
#include "webengineviewer_debug.h"
#include "downloadlocaldatabasethread.h"
#include <WebEngineViewer/CreatePhishingUrlDataBaseJob>
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
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob;
    job->moveToThread(this);
    job->setDataBaseDownloadNeeded(mCurrentDataBaseState.isEmpty() ? WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase : WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(mCurrentDataBaseState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &DownloadLocalDatabaseThread::slotDownloadDataBaseFinished);
    job->start();
    exec();
}

void DownloadLocalDatabaseThread::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{
    bool dataBaseOk = false;
    switch (status) {
    case CreatePhishingUrlDataBaseJob::InvalidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Invalid Data.";
        dataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::ValidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Valid Data.";
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
    WebEngineViewer::CreateDatabaseFileJob *job = new WebEngineViewer::CreateDatabaseFileJob;
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
