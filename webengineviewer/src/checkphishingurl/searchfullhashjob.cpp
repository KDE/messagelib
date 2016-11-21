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

#include "searchfullhashjob.h"

#include "checkphishingurlutil.h"
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <PimCommon/NetworkManager>
#include <QJsonDocument>
#include <webengineviewer_debug.h>

using namespace WebEngineViewer;

SearchFullHashJob::SearchFullHashJob(QObject *parent)
    : QObject(parent),
      mUseCompactJson(true)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &SearchFullHashJob::slotCheckUrlFinished);
    connect(mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &SearchFullHashJob::slotSslErrors);
}

SearchFullHashJob::~SearchFullHashJob()
{
}

void SearchFullHashJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void SearchFullHashJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void SearchFullHashJob::setUseCompactJson(bool useCompactJson)
{
    mUseCompactJson = useCompactJson;
}

void SearchFullHashJob::parse(const QByteArray &replyStr)
{
    /*

{
  "matches": [{
    "threatType":      "MALWARE",
    "platformType":    "WINDOWS",
    "threatEntryType": "URL",
    "threat": {
      "hash": "WwuJdQx48jP-4lxr4y2Sj82AWoxUVcIRDSk1PC9Rf-4="
    },
    "threatEntryMetadata": {
      "entries": [{
        "key": "bWFsd2FyZV90aHJlYXRfdHlwZQ==",  // base64-encoded "malware_threat_type"
        "value": "TEFORElORw=="  // base64-encoded "LANDING"
       }]
    },
    "cacheDuration": "300.000s"
  }, {
    "threatType":      "SOCIAL_ENGINEERING",
    "platformType":    "WINDOWS",
    "threatEntryType": "URL",
    "threat": {
      "hash": "771MOrRPMn6xPKlCrXx_CrR-wmCk0LgFFoSgGy7zUiA="
    },
    "threatEntryMetadata": {
      "entries": []
    },
    "cacheDuration": "300.000s"
  }],
  "minimumWaitDuration": "300.000s",
  "negativeCacheDuration": "300.000s"
}
*/
    QJsonDocument document = QJsonDocument::fromJson(replyStr);
    if (document.isNull()) {
        Q_EMIT result(WebEngineViewer::SearchFullHashJob::Unknown, mHash);
    } else {
        const QVariantMap answer = document.toVariant().toMap();
        if (answer.isEmpty()) {
            Q_EMIT result(WebEngineViewer::SearchFullHashJob::Ok, mHash);
            return;
        } else {
            const QVariantList info = answer.value(QStringLiteral("matches")).toList();
            //Implement multi match ?
            if (info.count() == 1) {
                const QVariantMap map = info.at(0).toMap();
                const QString threatTypeStr = map[QStringLiteral("threatType")].toString();
                if (threatTypeStr == QStringLiteral("MALWARE")) {
                    const QVariantMap urlMap = map[QStringLiteral("threat")].toMap();
                    if (urlMap.count() == 1) {
                        const QString hashStr = urlMap[QStringLiteral("hash")].toString();
                        //TODO
                        /*
                        if (urlMap[QStringLiteral("hash")].toString() == mHash.toString()) {
                            Q_EMIT result(WebEngineViewer::SearchFullHashJob::MalWare, mHash);
                            return;
                        }
                        */
                    }
                    const QVariantMap threatEntryMetadataMap = map[QStringLiteral("threatEntryMetadata")].toMap();
                    if (!threatEntryMetadataMap.isEmpty()) {
                        //TODO
                    }
                } else {
                    qWarning() << " SearchFullHashJob::parse threatTypeStr : " << threatTypeStr;
                }
            }
            Q_EMIT result(WebEngineViewer::SearchFullHashJob::Unknown, mHash);
        }
    }
}

void SearchFullHashJob::slotCheckUrlFinished(QNetworkReply *reply)
{
    parse(reply->readAll());
    reply->deleteLater();
    deleteLater();
}

void SearchFullHashJob::setSearchHash(const QByteArray &hash)
{
    mHash = hash;
}

QByteArray SearchFullHashJob::jsonRequest() const
{
    /*
{
  "client": {
    "clientId":      "yourcompanyname",
    "clientVersion": "1.5.2"
  },
  "clientStates": [
    "ChAIARABGAEiAzAwMSiAEDABEAE=",
    "ChAIAhABGAEiAzAwMSiAEDABEOgH"
  ],
  "threatInfo": {
    "threatTypes":      ["MALWARE", "SOCIAL_ENGINEERING"],
    "platformTypes":    ["WINDOWS"],
    "threatEntryTypes": ["URL"],
    "threatEntries": [
      {"hash": "WwuJdQ=="},
      {"hash": "771MOg=="},
      {"hash": "5eOrwQ=="}
    ]
  }
}
     */
    QVariantMap clientMap;
    QVariantMap map;

    clientMap.insert(QStringLiteral("clientId"), QStringLiteral("KDE"));
    clientMap.insert(QStringLiteral("clientVersion"), QStringLiteral("5.4.0")); //FIXME
    map.insert(QStringLiteral("client"), clientMap);

    //clientStates We can support multi database.
    const QVariantList clientStatesList = { mDatabaseHash };
    map.insert(QStringLiteral("clientStates"), clientStatesList);


    QVariantMap threatMap;
    const QVariantList platformList = { QStringLiteral("WINDOWS") };
    threatMap.insert(QStringLiteral("platformTypes"), platformList);

    const QVariantList threatTypesList = { QStringLiteral("MALWARE") };
    threatMap.insert(QStringLiteral("threatTypes"), threatTypesList);
    const QVariantList threatEntryTypesList = { QStringLiteral("URL") };
    threatMap.insert(QStringLiteral("threatEntryTypes"), threatEntryTypesList);

    QVariantList threatEntriesList;

    QVariantMap hashUrlMap;
    hashUrlMap.insert(QStringLiteral("hash"), mHash);
    threatEntriesList.append(hashUrlMap);

    threatMap.insert(QStringLiteral("threatEntries"), threatEntriesList);

    map.insert(QStringLiteral("threatInfo"), threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(mUseCompactJson ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void SearchFullHashJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT result(WebEngineViewer::SearchFullHashJob::BrokenNetwork, mHash);
        deleteLater();
    } else if (canStart()) {
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/fullHashes:find"));
        safeUrl.addQueryItem(QStringLiteral("key"), WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        Q_EMIT debugJson(baPostData);
        //curl -H "Content-Type: application/json" -X POST -d '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}' https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo

        QNetworkReply *reply = mNetworkAccessManager->post(request, baPostData);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &SearchFullHashJob::slotError);
    } else {
        Q_EMIT result(WebEngineViewer::SearchFullHashJob::InvalidUrl, mHash);
        deleteLater();
    }
}

void SearchFullHashJob::slotError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qCWarning(WEBENGINEVIEWER_LOG) << " error " << error << " error string : " << reply->errorString();
    reply->deleteLater();
    deleteLater();
}

bool SearchFullHashJob::canStart() const
{
    return !mHash.isEmpty() && !mDatabaseHash.isEmpty();
}

void SearchFullHashJob::setDatabaseState(const QString &hash)
{
    mDatabaseHash = hash;
}
