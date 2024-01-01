/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamexpandurljob.h"
#include "messageviewer_debug.h"

#include <PimCommon/BroadcastStatus>
#include <PimCommon/NetworkManager>

#include <KLocalizedString>

#include <QJsonDocument>
#include <QNetworkAccessManager>

using namespace MessageViewer;
class MessageViewer::ScamExpandUrlJobPrivate
{
public:
    ScamExpandUrlJobPrivate() = default;

    ~ScamExpandUrlJobPrivate() = default;

    QNetworkAccessManager *mNetworkAccessManager = nullptr;
};

ScamExpandUrlJob::ScamExpandUrlJob(QObject *parent)
    : QObject(parent)
    , d(new ScamExpandUrlJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);
    d->mNetworkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    d->mNetworkAccessManager->setStrictTransportSecurityEnabled(true);
    d->mNetworkAccessManager->enableStrictTransportSecurityStore(true);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &ScamExpandUrlJob::slotExpandFinished);
}

ScamExpandUrlJob::~ScamExpandUrlJob() = default;

void ScamExpandUrlJob::expandedUrl(const QUrl &url)
{
    if (!PimCommon::NetworkManager::self()->isOnline()) {
        PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("No network connection detected, we cannot expand url."));
        deleteLater();
        return;
    }
    const QUrl newUrl(QStringLiteral("https://lengthenurl.info/api/longurl/shorturl/?inputURL=%1&format=json").arg(url.url()));

    qCDebug(MESSAGEVIEWER_LOG) << " newUrl " << newUrl;
    QNetworkReply *reply = d->mNetworkAccessManager->get(QNetworkRequest(newUrl));
    reply->setProperty("shortUrl", url.url());
    connect(reply, &QNetworkReply::errorOccurred, this, &ScamExpandUrlJob::slotError);
}

void ScamExpandUrlJob::slotExpandFinished(QNetworkReply *reply)
{
    QUrl shortUrl;
    if (!reply->property("shortUrl").isNull()) {
        shortUrl.setUrl(reply->property("shortUrl").toString());
    }
    const QByteArray ba = reply->readAll();
    // qDebug() << " reply->readAll()" << ba;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba);
    reply->deleteLater();
    if (!jsonDoc.isNull()) {
        const QMap<QString, QVariant> map = jsonDoc.toVariant().toMap();
        QUrl longUrl;
        const QVariant longUrlVar = map.value(QStringLiteral("LongURL"));
        if (longUrlVar.isValid()) {
            longUrl.setUrl(longUrlVar.toString());
        } else {
            qCWarning(MESSAGEVIEWER_LOG) << "JSon is not correct" << ba;
            PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("Impossible to expand \'%1\'.", shortUrl.url()));
            deleteLater();
            return;
        }
        PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("Short url \'%1\' redirects to \'%2\'.", shortUrl.url(), longUrl.toDisplayString()));
    } else {
        PimCommon::BroadcastStatus::instance()->setStatusMsg(i18n("Impossible to expand \'%1\'.", shortUrl.url()));
    }
    deleteLater();
}

void ScamExpandUrlJob::slotError(QNetworkReply::NetworkError error)
{
    Q_EMIT expandUrlError(error);
    deleteLater();
}

#include "moc_scamexpandurljob.cpp"
