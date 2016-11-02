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

#include "checkphishingurljob.h"
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>

using namespace WebEngineViewer;

CheckPhishingUrlJob::CheckPhishingUrlJob(QObject *parent)
    : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CheckPhishingUrlJob::slotCheckUrlFinished);
    mNetworkConfigurationManager = new QNetworkConfigurationManager();
}

CheckPhishingUrlJob::~CheckPhishingUrlJob()
{
    delete mNetworkConfigurationManager;
}

void CheckPhishingUrlJob::slotCheckUrlFinished(QNetworkReply *reply)
{
    Q_UNUSED(reply);
    //TODO Q_EMIT result(MessageViewer::CheckPhishingUrlJob:: ?);
    deleteLater();
}

void CheckPhishingUrlJob::setUrl(const QUrl &url)
{
    mUrl = url;
}

void CheckPhishingUrlJob::start()
{
    if (canStart()) {
        const QString postRequest = createPostRequest();
        if (postRequest.isEmpty()) {
            Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Unknown);
            deleteLater();
        } else {
            QNetworkRequest request(QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/threatMatches:find")));
            //FIXME ?
            request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

            QUrlQuery postData;
            postData.addQueryItem(QStringLiteral("key"), QString() /*TODO*/);

            QNetworkReply *reply = mNetworkAccessManager->post(request, postData.query(QUrl::FullyEncoded).toUtf8());
            connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &CheckPhishingUrlJob::slotError);
        }
    } else {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Unknown);
        deleteLater();
    }
}

void CheckPhishingUrlJob::slotError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    //mErrorMsg = reply->errorString();
    //FIXME
    deleteLater();
}

bool CheckPhishingUrlJob::canStart() const
{
    if (!mNetworkConfigurationManager->isOnline()) {
        //TODO it's not online !
        return false;
    }

    return mUrl.isValid();
}

QString CheckPhishingUrlJob::createPostRequest()
{
    return {};
    //TODO
}
