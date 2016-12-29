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

#include "createdatabasefilejob.h"
#include "webengineviewer_debug.h"
#include "downloadlocaldatabasethread.h"
#include <WebEngineViewer/CreatePhishingUrlDataBaseJob>
using namespace WebEngineViewer;

DownloadLocalDatabaseThread::DownloadLocalDatabaseThread(QObject *parent)
    : QThread(parent),
      mDataBaseOk(false)
{

}

DownloadLocalDatabaseThread::~DownloadLocalDatabaseThread()
{

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
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setDataBaseDownloadNeeded(mCurrentDataBaseState.isEmpty() ? WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase : WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(mCurrentDataBaseState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &DownloadLocalDatabaseThread::slotDownloadDataBaseFinished);
    job->start();
}

void DownloadLocalDatabaseThread::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
        WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{
    //TODO install database
    switch (status) {
    case CreatePhishingUrlDataBaseJob::InvalidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Invalid Data.";
        mDataBaseOk = false;
        break;
    case CreatePhishingUrlDataBaseJob::ValidData:
        qCWarning(WEBENGINEVIEWER_LOG) << "Valid Data.";
        mDataBaseOk= true;
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
        deleteLater();
    }
}

void DownloadLocalDatabaseThread::setDatabaseFullPath(const QString &databaseFullPath)
{
    mDatabaseFullPath = databaseFullPath;
}

void DownloadLocalDatabaseThread::installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase)
{
    WebEngineViewer::CreateDatabaseFileJob *job = new WebEngineViewer::CreateDatabaseFileJob(this);
    job->setFileName(mDatabaseFullPath);
    job->setUpdateDataBaseInfo(infoDataBase);
    connect(job, &CreateDatabaseFileJob::finished, this, &DownloadLocalDatabaseThread::slotCreateDataBaseFileNameFinished);
    job->start();
}

void DownloadLocalDatabaseThread::slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr)
{
    Q_EMIT createDataBaseFinished(success, newClientState, minimumWaitDurationStr);
    //TODO q_emit

    deleteLater();
}
