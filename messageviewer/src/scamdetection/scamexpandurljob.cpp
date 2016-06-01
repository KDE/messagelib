/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#include "scamexpandurljob.h"
#include "messageviewer_debug.h"

#include <Libkdepim/BroadcastStatus>

#include <KLocalizedString>

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkReply>

using namespace MessageViewer;
class MessageViewer::ScamExpandUrlJobPrivate
{
public:
    ScamExpandUrlJobPrivate()
        : mNetworkAccessManager(Q_NULLPTR),
          mNetworkConfigurationManager(Q_NULLPTR)
    {

    }
    ~ScamExpandUrlJobPrivate()
    {
        delete mNetworkConfigurationManager;
    }

    QNetworkAccessManager *mNetworkAccessManager;
    QNetworkConfigurationManager *mNetworkConfigurationManager;
};

ScamExpandUrlJob::ScamExpandUrlJob(QObject *parent)
    : QObject(parent),
      d(new ScamExpandUrlJobPrivate)
{
    d->mNetworkAccessManager = new QNetworkAccessManager(this);

    connect(d->mNetworkAccessManager, &QNetworkAccessManager::finished, this, &ScamExpandUrlJob::slotExpandFinished);
    d->mNetworkConfigurationManager = new QNetworkConfigurationManager();
}

ScamExpandUrlJob::~ScamExpandUrlJob()
{
    delete d;
}

void ScamExpandUrlJob::expandedUrl(const QUrl &url)
{
    if (!d->mNetworkConfigurationManager->isOnline()) {
        KPIM::BroadcastStatus::instance()->setStatusMsg(i18n("No network connection detected, we cannot expand url."));
        deleteLater();
        return;
    }
    const QUrl newUrl = QStringLiteral("http://api.longurl.org/v2/expand?url=%1&format=json").arg(url.url());

    qCDebug(MESSAGEVIEWER_LOG) << " newUrl " << newUrl;
    QNetworkReply *reply = d->mNetworkAccessManager->get(QNetworkRequest(newUrl));
    reply->setProperty("shortUrl", url.url());
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, &ScamExpandUrlJob::slotError);
}

void ScamExpandUrlJob::slotExpandFinished(QNetworkReply *reply)
{
    QUrl shortUrl;
    if (!reply->property("shortUrl").isNull()) {
        shortUrl.setUrl(reply->property("shortUrl").toString());
    }
    QJsonDocument jsonDoc = QJsonDocument::fromBinaryData(reply->readAll());
    reply->deleteLater();
    if (!jsonDoc.isNull()) {
        const QMap<QString, QVariant> map = jsonDoc.toVariant().toMap();
        QUrl longUrl;
        if (map.contains(QStringLiteral("long-url"))) {
            longUrl.setUrl(map.value(QStringLiteral("long-url")).toString());
        } else {
            deleteLater();
            return;
        }
        KPIM::BroadcastStatus::instance()->setStatusMsg(i18n("Short url \'%1\' redirects to \'%2\'.", shortUrl.url(), longUrl.toDisplayString()));
    }
    deleteLater();
}

void ScamExpandUrlJob::slotError(QNetworkReply::NetworkError error)
{
    Q_EMIT expandUrlError(error);
    deleteLater();
}
