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

#include "checkphishingurlfromlocaldatabasejob.h"
#include <qcryptographichash.h>
#include "localdatabasemanager.h"

using namespace WebEngineViewer;

CheckPhishingUrlFromLocalDataBaseJob::CheckPhishingUrlFromLocalDataBaseJob(QObject *parent)
    : QObject(parent)
{

}

CheckPhishingUrlFromLocalDataBaseJob::~CheckPhishingUrlFromLocalDataBaseJob()
{

}

void CheckPhishingUrlFromLocalDataBaseJob::setCheckPhisingUrl(const QUrl &url)
{
    mUrl = url;
}

void CheckPhishingUrlFromLocalDataBaseJob::start()
{
    if (mUrl.isValid()) {
        Q_EMIT finished(mUrl, InvalidUrl);
        deleteLater();
    } else {
        QByteArray hash = createHash();
        connect(LocalDataBaseManager::self(), &LocalDataBaseManager::checkUrlFinished, this, &CheckPhishingUrlFromLocalDataBaseJob::slotCheckUrlFinished);
        //LocalDataBaseManager::self()->checkUrl(/*TODO*/);
    }
}

void CheckPhishingUrlFromLocalDataBaseJob::slotCheckUrlFinished(const QUrl &url, WebEngineViewer::LocalDataBaseManager::UrlStatus status)
{
    CheckPhishingUrlFromLocalDataBaseJob::UrlStatus currentStatus(CheckPhishingUrlFromLocalDataBaseJob::Unknown);
    switch(status) {
    case  WebEngineViewer::LocalDataBaseManager::Unknown:
        break;
    case  WebEngineViewer::LocalDataBaseManager::UrlOk:
        currentStatus = CheckPhishingUrlFromLocalDataBaseJob::Ok;
        break;
    case  WebEngineViewer::LocalDataBaseManager::Malware:
        currentStatus = CheckPhishingUrlFromLocalDataBaseJob::MalWare;
        break;
    }

    Q_EMIT finished(url,  currentStatus);
    deleteLater();
}


bool CheckPhishingUrlFromLocalDataBaseJob::canStart() const
{
    return mUrl.isValid();
}

QByteArray CheckPhishingUrlFromLocalDataBaseJob::createHash()
{
    return QCryptographicHash::hash(QByteArray() /*TODO use url*/, QCryptographicHash::Sha256);
}
