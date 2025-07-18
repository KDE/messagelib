/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "createphishingurldatabasejob.h"
using namespace Qt::Literals::StringLiterals;

#include "checkphishingurlutil.h"
#include "updatedatabaseinfo.h"
#include "webengineviewer_debug.h"
#include <PimCommon/NetworkManager>

#include <QJsonDocument>
#include <QUrlQuery>

using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_CreatePhishingUrlDataBaseJob = true;

class WebEngineViewer::CreatePhishingUrlDataBaseJobPrivate
{
public:
    [[nodiscard]] UpdateDataBaseInfo::CompressionType parseCompressionType(const QString &str);
    [[nodiscard]] RiceDeltaEncoding parseRiceDeltaEncoding(const QMap<QString, QVariant> &map);
    [[nodiscard]] QList<Removal> parseRemovals(const QVariantList &lst);
    [[nodiscard]] QList<Addition> parseAdditions(const QVariantList &lst);
    QString mDataBaseState;
    CreatePhishingUrlDataBaseJob::ContraintsCompressionType mContraintsCompressionType = CreatePhishingUrlDataBaseJob::RawCompression;
    CreatePhishingUrlDataBaseJob::DataBaseDownloadType mDataBaseDownloadNeeded = CreatePhishingUrlDataBaseJob::FullDataBase;
    QNetworkAccessManager *mNetworkAccessManager = nullptr;
};

CreatePhishingUrlDataBaseJob::CreatePhishingUrlDataBaseJob(QObject *parent)
    : QObject(parent)
    , d(new CreatePhishingUrlDataBaseJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);
    d->mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->mNetworkAccessManager->setStrictTransportSecurityEnabled(true);
    d->mNetworkAccessManager->enableStrictTransportSecurityStore(true);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &CreatePhishingUrlDataBaseJob::slotDownloadDataBaseFinished);
    connect(d->mNetworkAccessManager, &QNetworkAccessManager::sslErrors, this, &CreatePhishingUrlDataBaseJob::slotSslErrors);
}

CreatePhishingUrlDataBaseJob::~CreatePhishingUrlDataBaseJob() = default;

void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void CreatePhishingUrlDataBaseJob::slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error)" << error.count();
    reply->ignoreSslErrors(error);
}

void CreatePhishingUrlDataBaseJob::start()
{
    if (!PimCommon::NetworkManager::self()->isOnline()) {
        Q_EMIT finished(UpdateDataBaseInfo(), BrokenNetwork);
        deleteLater();
    } else {
        QUrlQuery query;
        query.addQueryItem(u"key"_s, WebEngineViewer::CheckPhishingUrlUtil::apiKey());
        QUrl safeUrl = QUrl(u"https://safebrowsing.googleapis.com/v4/threatListUpdates:fetch"_s);
        safeUrl.setQuery(query);
        // qCDebug(WEBENGINEVIEWER_LOG) << " safeUrl" << safeUrl;
        QNetworkRequest request(safeUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/json"_s);

        const QByteArray baPostData = jsonRequest();
        Q_EMIT debugJson(baPostData);
        qCDebug(WEBENGINEVIEWER_LOG) << " postData.toJson()" << baPostData;
        // curl -H "Content-Type: application/json" -X POST -d
        // '{"client":{"clientId":"KDE","clientVersion":"5.4.0"},"threatInfo":{"platformTypes":["WINDOWS"],"threatEntries":[{"url":"http://www.kde.org"}],"threatEntryTypes":["URL"],"threatTypes":["MALWARE"]}}'
        // https://safebrowsing.googleapis.com/v4/threatMatches:find?key=AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo
        QNetworkReply *reply = d->mNetworkAccessManager->post(request, baPostData);
        connect(reply, &QNetworkReply::errorOccurred, this, &CreatePhishingUrlDataBaseJob::slotError);
    }
}

void CreatePhishingUrlDataBaseJob::setDataBaseState(const QString &value)
{
    d->mDataBaseState = value;
}

void CreatePhishingUrlDataBaseJob::slotError(QNetworkReply::NetworkError error)
{
    auto reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << " error " << error << " error string : " << reply->errorString();
    reply->deleteLater();
    deleteLater();
}

QByteArray CreatePhishingUrlDataBaseJob::jsonRequest() const
{
#if 0
    {
        "client" : {
            "clientId" :       "yourcompanyname",
            "clientVersion" :  "1.5.2"
        },
        "listUpdateRequests" : [{
                                    "threatType" :      "MALWARE",
                                    "platformType" :    "WINDOWS",
                                    "threatEntryType" : "URL",
                                    "state" :           "Gg4IBBADIgYQgBAiAQEoAQ==",
                                    "constraints" : {
                                        "maxUpdateEntries" :      2048,
                                        "maxDatabaseEntries" :    4096,
                                        "region" :                "US",
                                        "supportedCompressions" : ["RAW"]
                                    }
                                }]
    }
#endif
    QVariantMap clientMap;
    QVariantMap map;

    clientMap.insert(u"clientId"_s, u"KDE"_s);
    clientMap.insert(u"clientVersion"_s, CheckPhishingUrlUtil::versionApps());
    map.insert(u"client"_s, clientMap);

    QVariantList listUpdateRequests;

    QVariantMap threatMap;
    threatMap.insert(u"platformType"_s, u"WINDOWS"_s);
    threatMap.insert(u"threatType"_s, u"MALWARE"_s);
    threatMap.insert(u"threatEntryType"_s, u"URL"_s);

    // Contrainsts
    QVariantMap contraintsMap;
    QVariantList contraintsCompressionList;
    QString compressionStr;
    switch (d->mContraintsCompressionType) {
    case RiceCompression:
        compressionStr = u"RICE"_s;
        break;
    case RawCompression:
        compressionStr = u"RAW"_s;
        break;
    }
    contraintsCompressionList.append(compressionStr);
    contraintsMap.insert(u"supportedCompressions"_s, contraintsCompressionList);
    threatMap.insert(u"constraints"_s, contraintsMap);

    // Define state when we want to define update database. Empty is full.
    switch (d->mDataBaseDownloadNeeded) {
    case FullDataBase:
        qCDebug(WEBENGINEVIEWER_LOG) << " full update";
        threatMap.insert(u"state"_s, QString());
        break;
    case UpdateDataBase:
        qCDebug(WEBENGINEVIEWER_LOG) << " update database";
        if (d->mDataBaseState.isEmpty()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Partial Download asked but database set is empty";
        }
        threatMap.insert(u"state"_s, d->mDataBaseState);
        break;
    }

    listUpdateRequests.append(threatMap);

    map.insert(u"listUpdateRequests"_s, listUpdateRequests);

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
    RiceDeltaEncoding riceDeltaEncodingTmp;
    QMap<QString, QVariant>::const_iterator riceHashesIt = map.cbegin();
    const QMap<QString, QVariant>::const_iterator riceHashesItEnd = map.cend();
    for (; riceHashesIt != riceHashesItEnd; ++riceHashesIt) {
        const QString key = riceHashesIt.key();
        if (key == QLatin1StringView("firstValue")) {
            riceDeltaEncodingTmp.firstValue = riceHashesIt.value().toByteArray();
        } else if (key == QLatin1StringView("riceParameter")) {
            riceDeltaEncodingTmp.riceParameter = riceHashesIt.value().toInt();
        } else if (key == QLatin1StringView("numEntries")) {
            riceDeltaEncodingTmp.numberEntries = riceHashesIt.value().toInt();
        } else if (key == QLatin1StringView("encodedData")) {
            riceDeltaEncodingTmp.encodingData = riceHashesIt.value().toByteArray();
        } else {
            qCDebug(WEBENGINEVIEWER_LOG) << " CreatePhishingUrlDataBaseJob::parseRiceDeltaEncoding unknown riceDeltaEncoding key " << key;
        }
    }
    return riceDeltaEncodingTmp;
}

QList<Addition> CreatePhishingUrlDataBaseJobPrivate::parseAdditions(const QVariantList &lst)
{
    QList<Addition> additionList;
    for (const QVariant &v : lst) {
        if (v.canConvert<QVariantMap>()) {
            QMapIterator<QString, QVariant> mapIt(v.toMap());
            Addition tmp;
            while (mapIt.hasNext()) {
                mapIt.next();
                const QString keyStr = mapIt.key();
                if (keyStr == QLatin1StringView("compressionType")) {
                    tmp.compressionType = parseCompressionType(mapIt.value().toString());
                } else if (keyStr == QLatin1StringView("riceHashes")) {
                    RiceDeltaEncoding riceDeltaEncodingTmp = parseRiceDeltaEncoding(mapIt.value().toMap());
                    if (riceDeltaEncodingTmp.isValid()) {
                        tmp.riceDeltaEncoding = riceDeltaEncodingTmp;
                    }
                } else if (keyStr == QLatin1StringView("rawHashes")) {
                    QMapIterator<QString, QVariant> rawHashesIt(mapIt.value().toMap());
                    while (rawHashesIt.hasNext()) {
                        rawHashesIt.next();
                        const QString key = rawHashesIt.key();
                        if (key == QLatin1StringView("rawHashes")) {
                            tmp.hashString = QByteArray::fromBase64(rawHashesIt.value().toByteArray());
                        } else if (key == QLatin1StringView("prefixSize")) {
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
    if (str == QLatin1StringView("COMPRESSION_TYPE_UNSPECIFIED")) {
        type = UpdateDataBaseInfo::UnknownCompression;
    } else if (str == QLatin1StringView("RICE")) {
        type = UpdateDataBaseInfo::RiceCompression;
    } else if (str == QLatin1StringView("RAW")) {
        type = UpdateDataBaseInfo::RawCompression;
    } else {
        qCWarning(WEBENGINEVIEWER_LOG) << "CreatePhishingUrlDataBaseJob::parseCompressionType unknown compression type " << str;
    }
    return type;
}

QList<Removal> CreatePhishingUrlDataBaseJobPrivate::parseRemovals(const QVariantList &lst)
{
    QList<Removal> removalList;
    for (const QVariant &v : lst) {
        if (v.canConvert<QVariantMap>()) {
            Removal tmp;
            QMapIterator<QString, QVariant> mapIt(v.toMap());
            while (mapIt.hasNext()) {
                mapIt.next();
                const QString keyStr = mapIt.key();
                if (keyStr == QLatin1StringView("compressionType")) {
                    tmp.compressionType = parseCompressionType(mapIt.value().toString());
                } else if (keyStr == QLatin1StringView("riceIndices")) {
                    RiceDeltaEncoding riceDeltaEncodingTmp = parseRiceDeltaEncoding(mapIt.value().toMap());
                    if (riceDeltaEncodingTmp.isValid()) {
                        tmp.riceDeltaEncoding = riceDeltaEncodingTmp;
                    }
                } else if (keyStr == QLatin1StringView("rawIndices")) {
                    const QVariantMap map = mapIt.value().toMap();
                    QMapIterator<QString, QVariant> rawIndicesIt(map);
                    while (rawIndicesIt.hasNext()) {
                        rawIndicesIt.next();
                        if (rawIndicesIt.key() == QLatin1StringView("indices")) {
                            const QVariantList rawList = rawIndicesIt.value().toList();
                            QList<quint32> indexList;
                            indexList.reserve(rawList.count());
                            for (const QVariant &var : rawList) {
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
                if (i.key() == QLatin1StringView("listUpdateResponses")) {
                    const QVariantList info = i.value().toList();
                    if (info.count() == 1) {
                        const QVariant infoVar = info.at(0);
                        if (infoVar.canConvert<QVariantMap>()) {
                            QMapIterator<QString, QVariant> mapIt(infoVar.toMap());
                            while (mapIt.hasNext()) {
                                mapIt.next();
                                const QString mapKey = mapIt.key();
                                if (mapKey == QLatin1StringView("additions")) {
                                    const QVariantList lst = mapIt.value().toList();
                                    const QList<Addition> addList = d->parseAdditions(lst);
                                    if (!addList.isEmpty()) {
                                        databaseInfo.additionList.append(addList);
                                    }
                                } else if (mapKey == QLatin1StringView("removals")) {
                                    const QVariantList lst = mapIt.value().toList();
                                    const QList<Removal> removeList = d->parseRemovals(lst);
                                    if (!removeList.isEmpty()) {
                                        databaseInfo.removalList.append(removeList);
                                    }
                                } else if (mapKey == QLatin1StringView("checksum")) {
                                    QMapIterator<QString, QVariant> mapCheckSum(mapIt.value().toMap());
                                    while (mapCheckSum.hasNext()) {
                                        mapCheckSum.next();
                                        if (mapCheckSum.key() == QLatin1StringView("sha256")) {
                                            databaseInfo.sha256 = mapCheckSum.value().toByteArray();
                                        } else {
                                            qCDebug(WEBENGINEVIEWER_LOG) << "Invalid checksum key" << mapCheckSum.key();
                                        }
                                    }
                                } else if (mapKey == QLatin1StringView("newClientState")) {
                                    databaseInfo.newClientState = mapIt.value().toString();
                                } else if (mapKey == QLatin1StringView("platformType")) {
                                    databaseInfo.platformType = mapIt.value().toString();
                                } else if (mapKey == QLatin1StringView("responseType")) {
                                    const QString str = mapIt.value().toString();
                                    if (str == QLatin1StringView("FULL_UPDATE")) {
                                        databaseInfo.responseType = UpdateDataBaseInfo::FullUpdate;
                                    } else if (str == QLatin1StringView("PARTIAL_UPDATE")) {
                                        databaseInfo.responseType = UpdateDataBaseInfo::PartialUpdate;
                                    } else {
                                        qCDebug(WEBENGINEVIEWER_LOG) << " unknown responsetype " << str;
                                        databaseInfo.responseType = UpdateDataBaseInfo::Unknown;
                                    }
                                } else if (mapKey == QLatin1StringView("threatEntryType")) {
                                    databaseInfo.threatEntryType = mapIt.value().toString();
                                } else if (mapKey == QLatin1StringView("threatType")) {
                                    databaseInfo.threatType = mapIt.value().toString();
                                } else {
                                    qCDebug(WEBENGINEVIEWER_LOG) << " unknown key " << mapKey;
                                }
                            }
                        }
                    }
                } else if (i.key() == QLatin1StringView("minimumWaitDuration")) {
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

#include "moc_createphishingurldatabasejob.cpp"
