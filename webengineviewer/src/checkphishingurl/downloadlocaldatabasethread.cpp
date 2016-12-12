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

#include "downloadlocaldatabasethread.h"
#include <WebEngineViewer/CreatePhishingUrlDataBaseJob>
using namespace WebEngineViewer;

DownloadLocalDatabaseThread::DownloadLocalDatabaseThread(QObject *parent)
    : QThread(parent)
{

}

DownloadLocalDatabaseThread::~DownloadLocalDatabaseThread()
{

}

void DownloadLocalDatabaseThread::setDataBaseState(const QString &value)
{
    mDataBaseState = value;
}

void DownloadLocalDatabaseThread::run()
{
    WebEngineViewer::CreatePhishingUrlDataBaseJob *job = new WebEngineViewer::CreatePhishingUrlDataBaseJob(this);
    job->setDataBaseDownloadNeeded(mDataBaseState.isEmpty() ? WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase : WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase);
    job->setDataBaseState(mDataBaseState);
    connect(job, &CreatePhishingUrlDataBaseJob::finished, this, &DownloadLocalDatabaseThread::slotDownloadDataBaseFinished);
    job->start();
}

void DownloadLocalDatabaseThread::slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
                                                               WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status)
{
    Q_EMIT downloadDataBaseFinished(infoDataBase, status);
    deleteLater();
}
