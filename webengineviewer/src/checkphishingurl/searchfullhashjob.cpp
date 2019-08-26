/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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
#include <QUrlQuery>
#include <webengineviewer_debug.h>

using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_SearchFullHashJob = true;

class WebEngineViewer::SearchFullHashJobPrivate
{
public:
    SearchFullHashJobPrivate()
    {
    }

    bool foundExactHash(const QList<QByteArray> &listLongHash);
    QHash<QByteArray, QByteArray> mHashs;
    QUrl mUrl;
    QStringList mDatabaseHashes;
    QNetworkAccessManager *mNetworkAccessManager = nullptr;
};

SearchFullHashJob::SearchFullHashJob(QObject *parent)
    : QObject(parent)
    , d(new SearchFullHashJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &SearchFullHashJob::slotCheckUrlFinished);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &SearchFullHashJob::slotSslErrors);
}

SearchFullHashJob::~SearchFullHashJob()
{
    delete d;
}

void SearchFullHashJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void SearchFullHashJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
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
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Unknown, d->mUrl);
    } else {
        const QVariantMap answer = document.toVariant().toMap();
        if (answer.isEmpty()) {
            Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Ok, d->mUrl);
            return;
        } else {
            const QVariantList info = answer.value(QStringLiteral("matches")).toList();
            //TODO
            //const QString minimumWaitDuration = answer.value(QStringLiteral("minimumWaitDuration")).toString();
            //const QString negativeCacheDuration = answer.value(QStringLiteral("negativeCacheDuration")).toString();
            //Implement multi match ?
            if (info.count() == 1) {
                const QVariantMap map = info.at(0).toMap();
                const QString threatTypeStr = map[QStringLiteral("threatType")].toString();

                //const QString cacheDuration = map[QStringLiteral("cacheDuration")].toString();

                if (threatTypeStr == QLatin1String("MALWARE")) {
                    const QVariantMap urlMap = map[QStringLiteral("threat")].toMap();
                    QList<QByteArray> hashList;
                    QMap<QString, QVariant>::const_iterator urlMapIt = urlMap.cbegin();
                    const QMap<QString, QVariant>::const_iterator urlMapItEnd = urlMap.cend();
                    for (; urlMapIt != urlMapItEnd; ++urlMapIt) {
                        const QByteArray hashStr = urlMapIt.value().toByteArray();
                        hashList << hashStr;
                    }

                    if (d->foundExactHash(hashList)) {
                        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::MalWare, d->mUrl);
                    } else {
                        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Unknown, d->mUrl);
                    }
                    const QVariantMap threatEntryMetadataMap = map[QStringLiteral("threatEntryMetadata")].toMap();
                    if (!threatEntryMetadataMap.isEmpty()) {
                        //TODO
                    }
                } else {
                    qCWarning(WEBENGINEVIEWER_LOG) << " SearchFullHashJob::parse threatTypeStr : " << threatTypeStr;
                }
            } else {
                qCWarning(WEBENGINEVIEWER_LOG) << " SearchFullHashJob::parse matches multi element : " << info.count();
                Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Unknown, d->mUrl);
            }
        }
    }
    deleteLater();
}

bool SearchFullHashJobPrivate::foundExactHash(const QList<QByteArray> &listLongHash)
{
    const QList<QByteArray> lstLongHash = mHashs.keys();
    for (const QByteArray &ba : lstLongHash) {
        if (listLongHash.contains(ba)) {
            return true;
        }
    }
    return false;
}

void SearchFullHashJob::slotCheckUrlFinished(QNetworkReply *reply)
{
    parse(reply->readAll());
    reply->deleteLater();
}

void SearchFullHashJob::setSearchHashs(const QHash<QByteArray, QByteArray> &hash)
{
    d->mHashs = hash;
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
    clientMap.insert(QStringLiteral("clientVersion"), CheckPhishingUrlUtil::versionApps());
    map.insert(QStringLiteral("client"), clientMap);

    //clientStates We can support multi database.
    QVariantList clientStatesList;
    for (const QString &str : qAsConst(d->mDatabaseHashes)) {
        if (!str.isEmpty()) {
            clientStatesList.append(str);
        }
    }
    map.insert(QStringLiteral("clientStates"), clientStatesList);

    QVariantMap threatMap;
    QVariantList platformList;
    platformList.append(QLatin1String("WINDOWS"));
    threatMap.insert(QStringLiteral("platformTypes"), platformList);

    const QVariantList threatTypesList = { QStringLiteral("MALWARE") };
    threatMap.insert(QStringLiteral("threatTypes"), threatTypesList);
    const QVariantList threatEntryTypesList = { QStringLiteral("URL") };
    threatMap.insert(QStringLiteral("threatEntryTypes"), threatEntryTypesList);

    QVariantList threatEntriesList;

    QVariantMap hashUrlMap;
    QHashIterator<QByteArray, QByteArray> i(d->mHashs);
    while (i.hasNext()) {
        i.next();
        hashUrlMap.insert(QStringLiteral("hash"), i.value());
    }
    threatEntriesList.append(hashUrlMap);

    threatMap.insert(QStringLiteral("threatEntries"), threatEntriesList);

    map.insert(QStringLiteral("threatInfo"), threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(webengineview_useCompactJson_SearchFullHashJob ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void SearchFullHashJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork, d->mUrl);
        deleteLater();
    } else if (canStart()) {
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("key"), WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/fullHashes:find"));
        safeUrl.setQuery(query);
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        //qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        Q_EMIT debugJson(baPostData);
        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error), this, &SearchFullHashJob::slotError);
    } else {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl, d->mUrl);
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
    return !d->mHashs.isEmpty() && !d->mDatabaseHashes.isEmpty() && !d->mUrl.isEmpty();
}

void SearchFullHashJob::setDatabaseState(const QStringList &hash)
{
    d->mDatabaseHashes = hash;
}

void SearchFullHashJob::setSearchFullHashForUrl(const QUrl &url)
{
    d->mUrl = url;
}
