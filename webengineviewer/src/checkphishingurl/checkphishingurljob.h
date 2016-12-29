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

#ifndef CHECKPHISHINGURLJOB_H
#define CHECKPHISHINGURLJOB_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include "webengineviewer_export.h"
#include "checkphishingurlutil.h"
namespace WebEngineViewer
{
class CheckPhishingUrlJobPrivate;
/* https://developers.google.com/safe-browsing/v4/lookup-api */
/* example http://malware.testing.google.test/testing/malware/ */
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlJob(QObject *parent = Q_NULLPTR);
    ~CheckPhishingUrlJob();

    void setUrl(const QUrl &url);

    void start();
    bool canStart() const;

    QByteArray jsonRequest() const;
    void parse(const QByteArray &replyStr);

Q_SIGNALS:
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url, uint verifyCacheAfterThisTime = 0);
    void debugJson(const QByteArray &ba);

private Q_SLOTS:
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    void slotError(QNetworkReply::NetworkError error);
    void slotCheckUrlFinished(QNetworkReply *reply);

private:
    CheckPhishingUrlJobPrivate *const d;
};
}
#endif // CHECKPHISHINGURLJOB_H
