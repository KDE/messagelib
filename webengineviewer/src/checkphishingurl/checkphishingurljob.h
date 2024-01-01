/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
    [[nodiscard]] bool canStart() const;

    [[nodiscard]] QByteArray jsonRequest() const;
    void parse(const QByteArray &replyStr);

Q_SIGNALS:
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url, uint verifyCacheAfterThisTime = 0);
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);
    WEBENGINEVIEWER_NO_EXPORT void slotCheckUrlFinished(QNetworkReply *reply);
    std::unique_ptr<CheckPhishingUrlJobPrivate> const d;
};
}
