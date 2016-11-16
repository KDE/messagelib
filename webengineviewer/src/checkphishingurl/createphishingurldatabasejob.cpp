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


#include "createphishingurldatabasejob.h"
#include "webengineviewer_debug.h"
#include <PimCommon/NetworkManager>

#include <QNetworkConfigurationManager>
#include <QJsonDocument>
#define DEBUG_JSON_REQUEST 1
using namespace WebEngineViewer;

CreatePhishingUrlDataBaseJob::CreatePhishingUrlDataBaseJob(QObject *parent)
    : QObject(parent),
      mDataBaseDownloadNeeded(FullDataBase)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CreatePhishingUrlDataBaseJob::slotDownloadDataBaseFinished);
    connect(mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CreatePhishingUrlDataBaseJob::slotSslErrors);
}

CreatePhishingUrlDataBaseJob::~CreatePhishingUrlDataBaseJob()
{

}

QString CreatePhishingUrlDataBaseJob::apiKey() const
{
    return QStringLiteral("AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo");
}


void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void CreatePhishingUrlDataBaseJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT finished();
        deleteLater();
    } else {
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/threatListUpdates:fetch"));
        safeUrl.addQueryItem(QStringLiteral("key"), apiKey());
        //qDebug() << " safeUrl" << safeUrl;
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        Q_EMIT debugJson(baPostData);
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        //curl -H "Content-Type: application/json" -X POST -d '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}' https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo
        QNetworkReply *reply = mNetworkAccessManager->post(request, baPostData);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &CreatePhishingUrlDataBaseJob::slotError);
    }
}

void CreatePhishingUrlDataBaseJob::setDataBaseState(const QString &value)
{
    mDataBaseState = value;
}

void CreatePhishingUrlDataBaseJob::slotError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << " error " << error << " error string : " << reply->errorString();
    reply->deleteLater();
    deleteLater();
}

QByteArray CreatePhishingUrlDataBaseJob::jsonRequest() const
{
#if 0
    {
      "client": {
        "clientId":       "yourcompanyname",
        "clientVersion":  "1.5.2"
      },
      "listUpdateRequests": [{
        "threatType":      "MALWARE",
        "platformType":    "WINDOWS",
        "threatEntryType": "URL",
        "state":           "Gg4IBBADIgYQgBAiAQEoAQ==",
        "constraints": {
          "maxUpdateEntries":      2048,
          "maxDatabaseEntries":    4096,
          "region":                "US",
          "supportedCompressions": ["RAW"]
        }
      }]
    }
#endif
    QVariantMap clientMap;
    QVariantMap map;

    clientMap.insert(QStringLiteral("clientId"), QStringLiteral("KDE"));
    clientMap.insert(QStringLiteral("clientVersion"), QStringLiteral("5.4.0")); //FIXME
    map.insert(QStringLiteral("client"), clientMap);

    QVariantList listUpdateRequests;

    QVariantMap threatMap;
    threatMap.insert(QStringLiteral("platformType"), QStringLiteral("WINDOWS"));
    threatMap.insert(QStringLiteral("threatType"), QStringLiteral("MALWARE"));
    threatMap.insert(QStringLiteral("threatEntryType"), QStringLiteral("URL"));

    //Define state when we want to define update database. Empty is full.
    switch(mDataBaseDownloadNeeded) {
    case FullDataBase:
        threatMap.insert(QStringLiteral("state"), QString());
        break;
    case UpdateDataBase:
        threatMap.insert(QStringLiteral("state"), mDataBaseState);
        break;
    }

    listUpdateRequests.append(threatMap);
    //TODO define contraints


    map.insert(QStringLiteral("listUpdateRequests"), listUpdateRequests);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
#ifdef DEBUG_JSON_REQUEST
    const QByteArray baPostData = postData.toJson();
#else
    const QByteArray baPostData = postData.toJson(QJsonDocument::Compact);
#endif
    //qDebug()<<" baPostData "<<baPostData;
    return baPostData;
}


void CreatePhishingUrlDataBaseJob::setDataBaseDownloadNeeded(CreatePhishingUrlDataBaseJob::DataBaseDownload type)
{
    mDataBaseDownloadNeeded = type;
}

void CreatePhishingUrlDataBaseJob::slotDownloadDataBaseFinished(QNetworkReply *reply)
{
    const QByteArray returnValue(reply->readAll());
    Q_EMIT debugJsonResult(returnValue);
    parseResult(returnValue);
    reply->deleteLater();
    //TODO deleteLater ?
}

void CreatePhishingUrlDataBaseJob::parseResult(const QByteArray &value)
{
    QJsonDocument document = QJsonDocument::fromJson(value);
    if (document.isNull()) {
        //TODO
        deleteLater();
    } else {
        const QVariantMap answer = document.toVariant().toMap();
        if (answer.isEmpty()) {
            //TODO
        } else {
            QMapIterator<QString, QVariant> i(answer);
            while (i.hasNext()) {
                i.next();
                qDebug()<<" i.key" << i.key();
                if (i.key() == QLatin1String("listUpdateResponses")) {
                    const QVariantList info = i.value().toList();
                    if (info.count() == 1) {
                        if (info.at(0).canConvert<QVariantMap>()) {
                            QMapIterator<QString, QVariant> mapIt(info.at(0).toMap());
                            while (mapIt.hasNext()) {
                                mapIt.next();
                                const QString mapKey = mapIt.key();
                                if (mapKey == QLatin1String("additions")) {

                                } else if (mapKey == QLatin1String("checksum")) {

                                } else if (mapKey == QLatin1String("newClientState")) {

                                } else if (mapKey == QLatin1String("newClientState")) {

                                } else if (mapKey == QLatin1String("platformType")) {

                                } else if (mapKey == QLatin1String("responseType")) {

                                } else if (mapKey == QLatin1String("threatEntryType")) {

                                } else if (mapKey == QLatin1String("threatType")) {

                                } else {
                                    qDebug() << " unknow key " << mapKey;
                                }

                            }
                        }
                    }
                } else if (i.key() == QLatin1String("minimumWaitDuration")) {

                    //qDebug() << " i.key()" << i.key();
                } else {
                    qDebug() <<" map key unknown " << i.key();
                }
            }
        }
    }
}
