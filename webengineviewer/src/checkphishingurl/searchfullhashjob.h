/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::SearchFullHashJob
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/SearchFullHashJob
 *
 * \brief The SearchFullHashJob class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT SearchFullHashJob : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a SearchFullHashJob object. */
    explicit SearchFullHashJob(QObject *parent = nullptr);
    /*! Destroys the SearchFullHashJob object. */
    ~SearchFullHashJob() override;

    /*! Starts the search job. */
    void start();
    /*! Returns whether the job can be started. */
    [[nodiscard]] bool canStart() const;

    /*! Sets the database state. */
    void setDatabaseState(const QStringList &hash);
    /*! Sets the URL for which to search the full hash. */
    void setSearchFullHashForUrl(const QUrl &url);

    /*! Returns the JSON request payload. */
    [[nodiscard]] QByteArray jsonRequest() const;
    /*! Parses the reply from the server. */
    void parse(const QByteArray &replyStr);

    /*! Sets the search hashes. */
    void setSearchHashs(const QHash<QByteArray, QByteArray> &hash);
Q_SIGNALS:
    /*! Emitted when the search result is available. */
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url);
    /*! Emitted for debugging JSON messages. */
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);
    WEBENGINEVIEWER_NO_EXPORT void slotCheckUrlFinished(QNetworkReply *reply);

    Q_DISABLE_COPY(SearchFullHashJob)
    std::unique_ptr<SearchFullHashJobPrivate> const d;
};
}
