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
#include "checkphishingurlutil.h"
#include "updatedatabaseinfo.h"
#include "webengineviewer_debug.h"
#include <PimCommon/NetworkManager>

#include <QNetworkConfigurationManager>
#include <QJsonDocument>

using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_CreatePhishingUrlDataBaseJob = true;

class WebEngineViewer::CreatePhishingUrlDataBaseJobPrivate
{
public:
    CreatePhishingUrlDataBaseJobPrivate()
        : mContraintsCompressionType(CreatePhishingUrlDataBaseJob::RawCompression),
          mDataBaseDownloadNeeded(CreatePhishingUrlDataBaseJob::FullDataBase),
          mNetworkAccessManager(Q_NULLPTR)
    {

    }
    UpdateDataBaseInfo::CompressionType parseCompressionType(const QString &str);
    RiceDeltaEncoding parseRiceDeltaEncoding(const QMap<QString, QVariant> &map);
    QVector<Removal> parseRemovals(const QVariantList &lst);
    QVector<Addition> parseAdditions(const QVariantList &lst);
    QString mDataBaseState;
    CreatePhishingUrlDataBaseJob::ContraintsCompressionType mContraintsCompressionType;
    CreatePhishingUrlDataBaseJob::DataBaseDownloadType mDataBaseDownloadNeeded;
    QNetworkAccessManager *mNetworkAccessManager;
};

CreatePhishingUrlDataBaseJob::CreatePhishingUrlDataBaseJob(QObject *parent)
    : QObject(parent),
      d(new CreatePhishingUrlDataBaseJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CreatePhishingUrlDataBaseJob::slotDownloadDataBaseFinished);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CreatePhishingUrlDataBaseJob::slotSslErrors);
}

CreatePhishingUrlDataBaseJob::~CreatePhishingUrlDataBaseJob()
{
    delete d;
}

void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void CreatePhishingUrlDataBaseJob::start()
{
    if (!PimCommon::NetworkManager::self()->networkConfigureManager()->isOnline()) {
        Q_EMIT finished(UpdateDataBaseInfo(), BrokenNetwork);
        deleteLater();
    } else {
        QUrl safeUrl = QUrl(QStringLiteral("https://safebrowsing.googleapis.com/v4/threatListUpdates:fetch"));
        safeUrl.addQueryItem(QStringLiteral("key"), WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        //qCDebug(WEBENGINEVIEWER_LOG) << " safeUrl" << safeUrl;
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

        const QByteArray baPostData = jsonRequest();
        Q_EMIT debugJson(baPostData);
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        //curl -H "Content-Type: application/json" -X POST -d '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}' https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo
        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &CreatePhishingUrlDataBaseJob::slotError);
    }
}

void CreatePhishingUrlDataBaseJob::setDataBaseState(const QString &value)
{
    d->mDataBaseState = value;
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
    clientMap.insert(QStringLiteral("clientVersion"), CheckPhishingUrlUtil::versionApps());
    map.insert(QStringLiteral("client"), clientMap);

    QVariantList listUpdateRequests;

    QVariantMap threatMap;
    threatMap.insert(QStringLiteral("platformType"), QStringLiteral("WINDOWS"));
    threatMap.insert(QStringLiteral("threatType"), QStringLiteral("MALWARE"));
    threatMap.insert(QStringLiteral("threatEntryType"), QStringLiteral("URL"));

    //Contrainsts
    QVariantMap contraintsMap;
    QVariantList contraintsCompressionList;
    QString compressionStr;
    switch (d->mContraintsCompressionType) {
    case RiceCompression:
        compressionStr = QStringLiteral("RICE");
        break;
    case RawCompression:
        compressionStr = QStringLiteral("RAW");
        break;
    }
    contraintsCompressionList.append(compressionStr);
    contraintsMap.insert(QStringLiteral("supportedCompressions"), contraintsCompressionList);
    threatMap.insert(QStringLiteral("constraints"), contraintsMap);

    //Define state when we want to define update database. Empty is full.
    switch (d->mDataBaseDownloadNeeded) {
    case FullDataBase:
        qCDebug(WEBENGINEVIEWER_LOG) << " full update";
        threatMap.insert(QStringLiteral("state"), QString());
        break;
    case UpdateDataBase:
        qCDebug(WEBENGINEVIEWER_LOG) << " update database";
        if (d->mDataBaseState.isEmpty()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Partial Download asked but database set is empty";
        }
        threatMap.insert(QStringLiteral("state"), d->mDataBaseState);
        break;
    }

    listUpdateRequests.append(threatMap);

    map.insert(QStringLiteral("listUpdateRequests"), listUpdateRequests);

    const QJsonDocument postData = QJsonDocument::fromVariant(map);
    const QByteArray baPostData = postData.toJson(webengineview_useCompactJson_CreatePhishingUrlDataBaseJob ? QJsonDocument::Compact : QJsonDocument::Indented);
    return baPostData;
}

void CreatePhishingUrlDataBaseJob::setDataBaseDownloadNeeded(CreatePhishingUrlDataBaseJob::DataBaseDownloadType type)
{
    d->mDataBaseDownloadNeeded = type;
}

void CreatePhishingUrlDataBaseJob::slotDownloadDataBaseFinished(QNetworkReply *reply)
{
    const QByteArray returnValue(reply->readAll());
    Q_EMIT debugJsonResult(returnValue);
    parseResult(returnValue);
    reply->deleteLater();
}

RiceDeltaEncoding CreatePhishingUrlDataBaseJobPrivate::parseRiceDeltaEncoding(const QMap<QString, QVariant> &map)
{
    QMapIterator<QString, QVariant> riceHashesIt(map);
    RiceDeltaEncoding riceDeltaEncodingTmp;
    while (riceHashesIt.hasNext()) {
        riceHashesIt.next();
        const QString key = riceHashesIt.key();
        if (key == QLatin1String("firstValue")) {
            riceDeltaEncodingTmp.firstValue = riceHashesIt.value().toByteArray();
        } else if (key == QLatin1String("riceParameter")) {
            riceDeltaEncodingTmp.riceParameter = riceHashesIt.value().toInt();
        } else if (key == QLatin1String("numEntries")) {
            riceDeltaEncodingTmp.numberEntries = riceHashesIt.value().toInt();
        } else if (key == QLatin1String("encodedData")) {
            riceDeltaEncodingTmp.encodingData = riceHashesIt.value().toByteArray();
        } else {
            qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseRiceDeltaEncoding unknown riceDeltaEncoding key " << key;
        }
    }
    return riceDeltaEncodingTmp;
}

QVector<Addition> CreatePhishingUrlDataBaseJobPrivate::parseAdditions(const QVariantList &lst)
{
    QVector<Addition> additionList;
    for (const QVariant &v : lst) {
        if (v.canConvert<QVariantMap>()) {
            QMapIterator<QString, QVariant> mapIt(v.toMap());
            Addition tmp;
            while (mapIt.hasNext()) {
                mapIt.next();
                const QString keyStr = mapIt.key();
                if (keyStr == QLatin1String("compressionType")) {
                    tmp.compressionType = parseCompressionType(mapIt.value().toString());
                } else if (keyStr == QLatin1String("riceHashes")) {
                    RiceDeltaEncoding riceDeltaEncodingTmp = parseRiceDeltaEncoding(mapIt.value().toMap());
                    if (riceDeltaEncodingTmp.isValid()) {
                        tmp.riceDeltaEncoding = riceDeltaEncodingTmp;
                    }
                } else if (keyStr == QLatin1String("rawHashes")) {
                    QMapIterator<QString, QVariant> rawHashesIt(mapIt.value().toMap());
                    while (rawHashesIt.hasNext()) {
                        rawHashesIt.next();
                        const QString key = rawHashesIt.key();
                        if (key == QLatin1String("rawHashes")) {
                            tmp.hashString = QByteArray::fromBase64(rawHashesIt.value().toByteArray());
                        } else if (key == QLatin1String("prefixSize")) {
                            tmp.prefixSize = rawHashesIt.value().toInt();
                        } else {
                            qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseAdditions unknown rawHashes key " << key;
                        }
                    }
                } else {
                    qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseAdditions unknown mapIt.key() " << keyStr;
                }
            }
            if (tmp.isValid()) {
                additionList.append(tmp);
            }
        } else {
            qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseAdditions not parsing type " << v.typeName();
        }
    }
    return additionList;
}

UpdateDataBaseInfo::CompressionType CreatePhishingUrlDataBaseJobPrivate::parseCompressionType(const QString &str)
{
    UpdateDataBaseInfo::CompressionType type(UpdateDataBaseInfo::UnknownCompression);
    if (str == QLatin1String("COMPRESSION_TYPE_UNSPECIFIED")) {
        type = UpdateDataBaseInfo::UnknownCompression;
    } else if (str == QLatin1String("RICE")) {
        type = UpdateDataBaseInfo::RiceCompression;
    } else if (str == QLatin1String("RAW")) {
        type = UpdateDataBaseInfo::RawCompression;
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "CreatePhishingUrlDataBaseJob::parseCompressionType unknown compression type " << str;
    }
    return type;
}

QVector<Removal> CreatePhishingUrlDataBaseJobPrivate::parseRemovals(const QVariantList &lst)
{
    QVector<Removal> removalList;
    for (const QVariant &v : lst) {
        if (v.canConvert<QVariantMap>()) {
            Removal tmp;
            QMapIterator<QString, QVariant> mapIt(v.toMap());
            while (mapIt.hasNext()) {
                mapIt.next();
                const QString keyStr = mapIt.key();
                if (keyStr == QLatin1String("compressionType")) {
                    tmp.compressionType = parseCompressionType(mapIt.value().toString());
                } else if (keyStr == QLatin1String("riceIndices")) {
                    RiceDeltaEncoding riceDeltaEncodingTmp = parseRiceDeltaEncoding(mapIt.value().toMap());
                    if (riceDeltaEncodingTmp.isValid()) {
                        tmp.riceDeltaEncoding = riceDeltaEncodingTmp;
                    }
                } else if (keyStr == QLatin1String("rawIndices")) {
                    const QVariantMap map = mapIt.value().toMap();
                    QMapIterator<QString, QVariant> rawIndicesIt(map);
                    while (rawIndicesIt.hasNext()) {
                        rawIndicesIt.next();
                        if (rawIndicesIt.key() == QStringLiteral("indices")) {
                            const QVariantList lst = rawIndicesIt.value().toList();
                            QList<quint32> indexList;
                            for (const QVariant &var : lst) {
                                indexList.append(var.toUInt());
                            }
                            tmp.indexes = indexList;
                        } else {
                            qCDebug(WEBENGINEVIEWER_LOG) << "rawIndicesIt.key() unknown " << rawIndicesIt.key();
                        }
                    }
                } else {
                    qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseRemovals unknown mapIt.key() " << keyStr;
                }
            }
            if (tmp.isValid()) {
                removalList.append(tmp);
            }
        } else {
            qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseRemovals not parsing type " << v.typeName();
        }
    }
    return removalList;
}

void CreatePhishingUrlDataBaseJob::parseResult(const QByteArray &value)
{
    UpdateDataBaseInfo databaseInfo;
    QJsonDocument document = QJsonDocument::fromJson(value);
    if (document.isNull()) {
        Q_EMIT finished(databaseInfo, InvalidData);
    } else {
        const QVariantMap answer = document.toVariant().toMap();
        if (answer.isEmpty()) {
            Q_EMIT finished(databaseInfo, InvalidData);
        } else {
            QMapIterator<QString, QVariant> i(answer);
            while (i.hasNext()) {
                i.next();
                if (i.key() == QLatin1String("listUpdateResponses")) {
                    const QVariantList info = i.value().toList();
                    if (info.count() == 1) {
                        const QVariant infoVar = info.at(0);
                        if (infoVar.canConvert<QVariantMap>()) {
                            QMapIterator<QString, QVariant> mapIt(infoVar.toMap());
                            while (mapIt.hasNext()) {
                                mapIt.next();
                                const QString mapKey = mapIt.key();
                                if (mapKey == QLatin1String("additions")) {
                                    const QVariantList lst = mapIt.value().toList();
                                    const QVector<Addition> addList = d->parseAdditions(lst);
                                    if (!addList.isEmpty()) {
                                        databaseInfo.additionList.append(addList);
                                    }
                                } else if (mapKey == QLatin1String("removals")) {
                                    const QVariantList lst = mapIt.value().toList();
                                    const QVector<Removal> removeList = d->parseRemovals(lst);
                                    if (!removeList.isEmpty()) {
                                        databaseInfo.removalList.append(removeList);
                                    }
                                } else if (mapKey == QLatin1String("checksum")) {
                                    QMapIterator<QString, QVariant> mapCheckSum(mapIt.value().toMap());
                                    while (mapCheckSum.hasNext()) {
                                        mapCheckSum.next();
                                        if (mapCheckSum.key() == QLatin1String("sha256")) {
                                            databaseInfo.sha256 = mapCheckSum.value().toByteArray();
                                        } else {
                                            qCDebug(WEBENGINEVIEWER_LOG) << "Invalid checksum key" << mapCheckSum.key();
                                        }
                                    }
                                } else if (mapKey == QLatin1String("newClientState")) {
                                    databaseInfo.newClientState = mapIt.value().toString();
                                } else if (mapKey == QLatin1String("platformType")) {
                                    databaseInfo.platformType = mapIt.value().toString();
                                } else if (mapKey == QLatin1String("responseType")) {
                                    const QString str = mapIt.value().toString();
                                    if (str == QLatin1String("FULL_UPDATE")) {
                                        databaseInfo.responseType = UpdateDataBaseInfo::FullUpdate;
                                    } else if (str == QLatin1String("PARTIAL_UPDATE")) {
                                        databaseInfo.responseType = UpdateDataBaseInfo::PartialUpdate;
                                    } else {
                                        qCDebug(WEBENGINEVIEWER_LOG) << " unknow responsetype " << str;
                                        databaseInfo.responseType = UpdateDataBaseInfo::Unknown;
                                    }
                                } else if (mapKey == QLatin1String("threatEntryType")) {
                                    databaseInfo.threatEntryType = mapIt.value().toString();
                                } else if (mapKey == QLatin1String("threatType")) {
                                    databaseInfo.threatType = mapIt.value().toString();
                                } else {
                                    qCDebug(WEBENGINEVIEWER_LOG) << " unknow key " << mapKey;
                                }
                            }
                        }
                    }
                } else if (i.key() == QLatin1String("minimumWaitDuration")) {
                    databaseInfo.minimumWaitDuration = i.value().toString();
                } else {
                    qCDebug(WEBENGINEVIEWER_LOG) << " map key unknown " << i.key();
                }
            }
            Q_EMIT finished(databaseInfo, ValidData);
        }
    }
    deleteLater();
}

void CreatePhishingUrlDataBaseJob::setContraintsCompressionType(CreatePhishingUrlDataBaseJob::ContraintsCompressionType type)
{
    d->mContraintsCompressionType = type;
}

