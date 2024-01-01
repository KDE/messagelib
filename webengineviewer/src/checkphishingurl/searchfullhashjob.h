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
class SearchFullHashJobPrivate;
/* https://developers.google.com/safe-browsing/v4/update-api */
/**
 * @brief The SearchFullHashJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT SearchFullHashJob : public QObject
{
    Q_OBJECT
public:
    explicit SearchFullHashJob(QObject *parent = nullptr);
    ~SearchFullHashJob() override;

    void start();
    [[nodiscard]] bool canStart() const;

    void setDatabaseState(const QStringList &hash);
    void setSearchFullHashForUrl(const QUrl &url);

    [[nodiscard]] QByteArray jsonRequest() const;
    void parse(const QByteArray &replyStr);

    void setSearchHashs(const QHash<QByteArray, QByteArray> &hash);
Q_SIGNALS:
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url);
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);
    WEBENGINEVIEWER_NO_EXPORT void slotCheckUrlFinished(QNetworkReply *reply);

    Q_DISABLE_COPY(SearchFullHashJob)
    std::unique_ptr<SearchFullHashJobPrivate> const d;
};
}
