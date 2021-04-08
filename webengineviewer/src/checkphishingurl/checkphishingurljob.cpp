/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurljob.h"
#include <PimCommon/NetworkManager>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QUrlQuery>
#include <webengineviewer_debug.h>
using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson = true;

class WebEngineViewer::CheckPhishingUrlJobPrivate
{
public:
    CheckPhishingUrlJobPrivate()
    {
    }

    QUrl mUrl;
    QNetworkAccessManager *mNetworkAccessManager = nullptr;
};

CheckPhishingUrlJob::CheckPhishingUrlJob(QObject *parent)
    : QObject(parent)
    , d(new WebEngineViewer::CheckPhishingUrlJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);
    d->mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->mNetworkAccessManager->setStrictTransportSecurityEnabled(true);
    d->mNetworkAccessManager->enableStrictTransportSecurityStore(true);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CheckPhishingUrlJob::slotCheckUrlFinished);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CheckPhishingUrlJob::slotSslErrors);
}

CheckPhishingUrlJob::~CheckPhishingUrlJob()
{
    delete d;
}

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
            const QVariantList info = answer.value(QStringLiteral("matches")).toList();
            if (info.count() == 1) {
                const QVariantMap map = info.at(0).toMap();
                const QString threatTypeStr = map[QStringLiteral("threatType")].toString();
                const QString cacheDuration = map[QStringLiteral("cacheDuration")].toString();
                uint verifyCacheAfterThisTime = 0;
                if (!cacheDuration.isEmpty()) {
                    double cacheDurationValue = WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(cacheDuration);
                    if (cacheDurationValue > 0) {
                        verifyCacheAfterThisTime = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(cacheDurationValue);
                    }
                }
                if (threatTypeStr == QLatin1String("MALWARE")) {
                    const QVariantMap urlMap = map[QStringLiteral("threat")].toMap();
                    if (urlMap.count() == 1) {
                        if (urlMap[QStringLiteral("url")].toString() == d->mUrl.toString()) {
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

    clientMap.insert(QStringLiteral("clientId"), QStringLiteral("KDE"));
    clientMap.insert(QStringLiteral("clientVersion"), CheckPhishingUrlUtil::versionApps());
    map.insert(QStringLiteral("client"), clientMap);

    QVariantMap threatMap;
    const QVariantList platformList = {QStringLiteral("WINDOWS")};
    threatMap.insert(QStringLiteral("platformTypes"), platformList);

    const QVariantList threatTypesList = {QStringLiteral("MALWARE")};
    threatMap.insert(QStringLiteral("threatTypes"), threatTypesList);
    const QVariantList threatEntryTypesList = {QStringLiteral("URL")};
    threatMap.insert(QStringLiteral("threatEntryTypes"), threatEntryTypesList);
    QVariantList threatEntriesList;
    QVariantMap urlMap;
    urlMap.insert(QStringLiteral("url"), d->mUrl.toString());
    threatEntriesList.append(urlMap);
    threatMap.insert(QStringLiteral("threatEntries"), threatEntriesList);

    map.insert(QStringLiteral("threatInfo"), threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(webengineview_useCompactJson ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void CheckPhishingUrlJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlUtil::BrokenNetwork, d->mUrl);
        deleteLater();
    } else if (canStart()) {
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("key"), WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/threatMatches:find"));
        safeUrl.setQuery(query);
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        Q_EMIT debugJson(baPostData);
        // curl -H "Content-Type: application/json" -X POST -d
        // '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}'
        // https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo

        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::errorOccurred), this, &CheckPhishingUrlJob::slotError);
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
