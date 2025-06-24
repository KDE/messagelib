/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurljob.h"
using namespace Qt::Literals::StringLiterals;

#include <PimCommon/NetworkManager>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <webengineviewer_debug.h>
using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson = true;

class WebEngineViewer::CheckPhishingUrlJobPrivate
{
public:
    explicit CheckPhishingUrlJobPrivate(CheckPhishingUrlJob *q)
        : mNetworkAccessManager(new QNetworkAccessManager(q))
    {
    }

    QUrl mUrl;
    QNetworkAccessManager *const mNetworkAccessManager;
};

CheckPhishingUrlJob::CheckPhishingUrlJob(QObject *parent)
    : QObject(parent)
    , d(new WebEngineViewer::CheckPhishingUrlJobPrivate(this))
{
    d->mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->mNetworkAccessManager->setStrictTransportSecurityEnabled(true);
    d->mNetworkAccessManager->enableStrictTransportSecurityStore(true);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CheckPhishingUrlJob::slotCheckUrlFinished);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CheckPhishingUrlJob::slotSslErrors);
}

CheckPhishingUrlJob::~CheckPhishingUrlJob() = default;

void CheckPhishingUrlJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void CheckPhishingUrlJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void CheckPhishingUrlJob::parse(const QByteArray &replyStr)
{
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
            if (info.count() == 1) {
                const QVariantMap map = info.at(0).toMap();
                const QString threatTypeStr = map[u"threatType"_s].toString();
                const QString cacheDuration = map[u"cacheDuration"_s].toString();
                uint verifyCacheAfterThisTime = 0;
                if (!cacheDuration.isEmpty()) {
                    double cacheDurationValue = WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(cacheDuration);
                    if (cacheDurationValue > 0) {
                        verifyCacheAfterThisTime = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(cacheDurationValue);
                    }
                }
                if (threatTypeStr == QLatin1StringView("MALWARE")) {
                    const QVariantMap urlMap = map[u"threat"_s].toMap();
                    if (urlMap.count() == 1) {
                        if (urlMap[u"url"_s].toString() == d->mUrl.toString()) {
                            Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::MalWare, d->mUrl, verifyCacheAfterThisTime);
                            return;
                        }
                    }
                } else {
                    qCWarning(WEBENGINEVIEWER_LOG) << " CheckPhishingUrlJob::parse threatTypeStr : " << threatTypeStr;
                }
            }
            Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::Unknown, d->mUrl);
        }
    }
}

void CheckPhishingUrlJob::slotCheckUrlFinished(QNetworkReply *reply)
{
    parse(reply->readAll());
    reply->deleteLater();
    deleteLater();
}

void CheckPhishingUrlJob::setUrl(const QUrl &url)
{
    d->mUrl = url;
}

QByteArray CheckPhishingUrlJob::jsonRequest() const
{
    QVariantMap clientMap;
    QVariantMap map;

    clientMap.insert(u"clientId"_s, u"KDE"_s);
    clientMap.insert(u"clientVersion"_s, CheckPhishingUrlUtil::versionApps());
    map.insert(u"client"_s, clientMap);

    QVariantMap threatMap;
    const QVariantList platformList = {u"WINDOWS"_s};
    threatMap.insert(u"platformTypes"_s, platformList);

    const QVariantList threatTypesList = {u"MALWARE"_s};
    threatMap.insert(u"threatTypes"_s, threatTypesList);
    const QVariantList threatEntryTypesList = {u"URL"_s};
    threatMap.insert(u"threatEntryTypes"_s, threatEntryTypesList);
    QVariantList threatEntriesList;
    QVariantMap urlMap;
    urlMap.insert(u"url"_s, d->mUrl.toString());
    threatEntriesList.append(urlMap);
    threatMap.insert(u"threatEntries"_s, threatEntriesList);

    map.insert(u"threatInfo"_s, threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(webengineview_useCompactJson ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void CheckPhishingUrlJob::start()
{
    if (!PimCommon::NetworkManager::self()->isOnline()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork, d->mUrl);
        deleteLater();
    } else if (canStart()) {
        QUrlQuery query;
        query.addQueryItem(u"key"_s, WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QUrl safeUrl = QUrl(u"https://safebrowsing.googleapis.com/v4/threatMatches:find"_s);
        safeUrl.setQuery(query);
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);

        const QByteArray baPostData = jsonRequest();
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        Q_EMIT debugJson(baPostData);
        // curl -H "Content-Type: application/json" -X POST -d
        // '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}'
        // https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo

        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, &QNetworkReply::errorOccurred, this, &CheckPhishingUrlJob::slotError);
    } else {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::InvalidUrl, d->mUrl);
        deleteLater();
    }
}

void CheckPhishingUrlJob::slotError(QNetworkReply::NetworkError error)
{
    auto reply = qobject_cast<QNetworkReply *>(sender());
    qCWarning(WEBENGINEVIEWER_LOG) << " error " << error << " error string : " << reply->errorString();
    reply->deleteLater();
    deleteLater();
}

bool CheckPhishingUrlJob::canStart() const
{
    return d->mUrl.isValid();
}

#include "moc_checkphishingurljob.cpp"
