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
#include <PimCommon/NetworkManager>
#include <QJsonDocument>
#include <webengineviewer_debug.h>

using namespace WebEngineViewer;

CheckPhishingUrlJob::CheckPhishingUrlJob(QObject *parent)
    : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CheckPhishingUrlJob::slotCheckUrlFinished);
    connect(mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CheckPhishingUrlJob::slotSslErrors);
}

CheckPhishingUrlJob::~CheckPhishingUrlJob()
{
}

void CheckPhishingUrlJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void StorageServiceAbstractJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

QString CheckPhishingUrlJob::apiKey() const
{
    return QStringLiteral("AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo");
}

void CheckPhishingUrlJob::parse(const QByteArray &replyStr)
{
    QJsonDocument document = QJsonDocument::fromJson(replyStr);
    if (document.isNull()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Unknown, mUrl);
    } else {
        const QVariantMap answer = document.toVariant().toMap();
        if (answer.isEmpty()) {
            Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Ok, mUrl);
            return;
        } else {
            const QVariantList info = answer.value(QStringLiteral("matches")).toList();
            if (info.count() == 1) {
                const QVariantMap map = info.at(0).toMap();
                if (map[QStringLiteral("threatType")] == QStringLiteral("MALWARE")) {
                    const QVariantMap urlMap = map[QStringLiteral("threat")].toMap();
                    if (urlMap.count() == 1) {
                        if (urlMap[QStringLiteral("url")].toString() == mUrl.toString()) {
                            Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::MalWare, mUrl);
                            return;
                        }
                    }
                }
            }
            Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Unknown, mUrl);
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
    mUrl = url;
}

QByteArray CheckPhishingUrlJob::jsonRequest() const
{
#if 0
        {
            "client": {
                "clientId":      "yourcompanyname",
                "clientVersion": "1.5.2"
            },
            "threatInfo": {
                "threatTypes":      ["MALWARE", "SOCIAL_ENGINEERING"],
                "platformTypes":    ["WINDOWS"],
                "threatEntryTypes": ["URL"],
                "threatEntries": [
                {"url": "http://www.urltocheck1.org/"},
                {"url": "http://www.urltocheck2.org/"},
                {"url": "http://www.urltocheck3.com/"}
                ]
            }
        }
#endif
    QVariantMap clientMap;
    QVariantMap map;

    clientMap.insert(QStringLiteral("clientId"), QStringLiteral("KDE"));
    clientMap.insert(QStringLiteral("clientVersion"), QStringLiteral("5.4.0")); //FIXME
    map.insert(QStringLiteral("client"), clientMap);

    QVariantMap threatMap;
    const QVariantList platformList = { QStringLiteral("WINDOWS") };
    threatMap.insert(QStringLiteral("platformTypes"), platformList);

    const QVariantList threatTypesList = { QStringLiteral("MALWARE") };
    threatMap.insert(QStringLiteral("threatTypes"), threatTypesList);
    const QVariantList threatEntryTypesList = { QStringLiteral("URL") };
    threatMap.insert(QStringLiteral("threatEntryTypes"), threatEntryTypesList);
    QVariantList threatEntriesList;
    QVariantMap urlMap;
    urlMap.insert(QStringLiteral("url"), mUrl.toString());
    threatEntriesList.append(urlMap);
    threatMap.insert(QStringLiteral("threatEntries"), threatEntriesList);

    map.insert(QStringLiteral("threatInfo"), threatMap);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson();
    return baPostData;
}

void CheckPhishingUrlJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::BrokenNetwork, mUrl);
        deleteLater();
    } else if (canStart()) {
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/threatMatches:find"));
        safeUrl.addQueryItem(QStringLiteral("key"), apiKey());
        //qDebug() << " safeUrl" << safeUrl;
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        qCDebug(WEBENGINEVIEWER_LOG) <<" postData.toJson()"<<baPostData;
        Q_EMIT debugJson(baPostData);
        //curl -H "Content-Type: application/json" -X POST -d '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}' https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo

        QNetworkReply *reply = mNetworkAccessManager->post(request, baPostData);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &CheckPhishingUrlJob::slotError);
    } else {
        Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::InvalidUrl, mUrl);
        deleteLater();
    }
}

void CheckPhishingUrlJob::slotError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qDebug()<<" error "<<error << " error string : "<< reply->errorString();
    reply->deleteLater();
    //Q_EMIT result(WebEngineViewer::CheckPhishingUrlJob::Unknown, mUrl);
    deleteLater();
}

bool CheckPhishingUrlJob::canStart() const
{
    return mUrl.isValid();
}
