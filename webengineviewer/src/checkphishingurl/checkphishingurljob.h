/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "checkphishingurlutil.h"
#include "webengineviewer_export.h"
#include <QNetworkReply>
#include <QObject>
#include <QUrl>
namespace WebEngineViewer
{
class CheckPhishingUrlJobPrivate;
/* https://developers.google.com/safe-browsing/v4/lookup-api */
/* example http://malware.testing.google.test/testing/malware/ */
/**
 * @brief The CheckPhishingUrlJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlJob(QObject *parent = nullptr);
    ~CheckPhishingUrlJob() override;

    void setUrl(const QUrl &url);

    void start();
    Q_REQUIRED_RESULT bool canStart() const;

    Q_REQUIRED_RESULT QByteArray jsonRequest() const;
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
