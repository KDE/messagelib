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
class CheckPhishingUrlJobPrivate;
/* https://developers.google.com/safe-browsing/v4/lookup-api */
/* example http://malware.testing.google.test/testing/malware/ */
/*!
 * \class WebEngineViewer::CheckPhishingUrlJob
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/CheckPhishingUrlJob
 *
 * \brief The CheckPhishingUrlJob class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlJob : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a CheckPhishingUrlJob object. */
    explicit CheckPhishingUrlJob(QObject *parent = nullptr);
    /*! Destroys the CheckPhishingUrlJob object. */
    ~CheckPhishingUrlJob() override;

    /*! Sets the URL to check. */
    void setUrl(const QUrl &url);

    /*! Starts the URL check job. */
    void start();
    /*! Returns whether the job can be started. */
    [[nodiscard]] bool canStart() const;

    /*! Returns the JSON request payload. */
    [[nodiscard]] QByteArray jsonRequest() const;
    /*! Parses the response from the server. */
    void parse(const QByteArray &replyStr);

Q_SIGNALS:
    /*! Emitted when the URL check result is available. */
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url, uint verifyCacheAfterThisTime = 0);
    /*! Emitted for debugging JSON messages. */
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);
    WEBENGINEVIEWER_NO_EXPORT void slotCheckUrlFinished(QNetworkReply *reply);
    std::unique_ptr<CheckPhishingUrlJobPrivate> const d;
};
}
