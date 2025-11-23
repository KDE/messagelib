/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "searchfullhashjob.h"
using namespace Qt::Literals::StringLiterals;

#include <PimCommon/NetworkManager>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <webengineviewer_debug.h>

using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_SearchFullHashJob = true;

class WebEngineViewer::SearchFullHashJobPrivate
{
public:
    SearchFullHashJobPrivate() = default;

    [[nodiscard]] bool foundExactHash(const QList<QByteArray> &listLongHash);
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
    d->mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->mNetworkAccessManager->setStrictTransportSecurityEnabled(true);
    d->mNetworkAccessManager->enableStrictTransportSecurityStore(true);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &SearchFullHashJob::slotCheckUrlFinished);
}

SearchFullHashJob::~SearchFullHashJob() = default;

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
            const QVariantList info = answer.value(u"matches"_s).toList();
            // TODO
            // const QString minimumWaitDuration = answer.value(u"minimumWaitDuration"_s).toString();
            // const QString negativeCacheDuration = answer.value(u"negativeCacheDuration"_s).toString();
            // Implement multi match ?
            const auto numberOfInfo{info.count()};
            if (numberOfInfo == 1) {
                const QVariantMap map = info.at(0).toMap();
                const QString threatTypeStr = map[u"threatType"_s].toString();

                // const QString cacheDuration = map[u"cacheDuration"_s].toString();

                if (threatTypeStr == QLatin1StringView("MALWARE")) {
                    const QVariantMap urlMap = map[u"threat"_s].toMap();
                    QList<QByteArray> hashList;
                    QMap<QString, QVariant>::const_iterator urlMapIt = urlMap.cbegin();
                    const QMap<QString, QVariant>::const_iterator urlMapItEnd = urlMap.cend();
                    hashList.reserve(urlMap.count());
                    for (; urlMapIt != urlMapItEnd; ++urlMapIt) {
                        const QByteArray hashStr = urlMapIt.value().toByteArray();
                        hashList << hashStr;
                    }

                    if (d->foundExactHash(hashList)) {
                        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::MalWare, d->mUrl);
                    } else {
                        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Unknown, d->mUrl);
                    }
                    const QVariantMap threatEntryMetadataMap = map[u"threatEntryMetadata"_s].toMap();
                    if (!threatEntryMetadataMap.isEmpty()) {
                        // TODO
                    }
                } else {
                    qCWarning(WEBENGINEVIEWER_LOG) << " SearchFullHashJob::parse threatTypeStr : " << threatTypeStr;
                }
            } else if (numberOfInfo == 0) {
                Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Ok, d->mUrl);
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

    clientMap.insert(u"clientId"_s, u"KDE"_s);
    clientMap.insert(u"clientVersion"_s, CheckPhishingUrlUtil::versionApps());
    map.insert(u"client"_s, clientMap);

    // clientStates We can support multi database.
    QVariantList clientStatesList;
    for (const QString &str : std::as_const(d->mDatabaseHashes)) {
        if (!str.isEmpty()) {
            clientStatesList.append(str);
        }
    }
    map.insert(u"clientStates"_s, clientStatesList);

    QVariantMap threatMap;
    QVariantList platformList;
    platformList.append(QLatin1StringView("WINDOWS"));
    threatMap.insert(u"platformTypes"_s, platformList);

    const QVariantList threatTypesList = {u"MALWARE"_s};
    threatMap.insert(u"threatTypes"_s, threatTypesList);
    const QVariantList threatEntryTypesList = {u"URL"_s};
    threatMap.insert(u"threatEntryTypes"_s, threatEntryTypesList);

    QVariantList threatEntriesList;

    QVariantMap hashUrlMap;
    QHashIterator<QByteArray, QByteArray> i(d->mHashs);
    while (i.hasNext()) {
        i.next();
        hashUrlMap.insert(u"hash"_s, i.value());
    }
    threatEntriesList.append(hashUrlMap);

    threatMap.insert(u"threatEntries"_s, threatEntriesList);

    map.insert(u"threatInfo"_s, threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(webengineview_useCompactJson_SearchFullHashJob ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void SearchFullHashJob::start()
{
    if (!PimCommon::NetworkManager::self()->isOnline()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork, d->mUrl);
        deleteLater();
    } else if (canStart()) {
        QUrlQuery query;
        query.addQueryItem(u"key"_s, WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QUrl safeUrl = QUrl(u"https://safebrowsing.googleapis.com/v4/fullHashes:find"_s);
        safeUrl.setQuery(query);
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);

        const QByteArray baPostData = jsonRequest();
        // qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        Q_EMIT debugJson(baPostData);
        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, &QNetworkReply::errorOccurred, this, &SearchFullHashJob::slotError);
    } else {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl, d->mUrl);
        deleteLater();
    }
}

void SearchFullHashJob::slotError(QNetworkReply::NetworkError error)
{
    auto reply = qobject_cast<QNetworkReply *>(sender());
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

#include "moc_searchfullhashjob.cpp"
