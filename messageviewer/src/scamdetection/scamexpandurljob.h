/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include <QNetworkReply>
#include <QObject>
namespace MessageViewer
{
class ScamExpandUrlJobPrivate;
/**
 * @brief The ScamExpandUrlJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamExpandUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit ScamExpandUrlJob(QObject *parent = nullptr);
    ~ScamExpandUrlJob() override;

    void expandedUrl(const QUrl &url);
Q_SIGNALS:
    void urlExpanded(const QString &shortUrl, const QString &expandedUrl);
    void expandUrlError(QNetworkReply::NetworkError error);

private:
    void slotError(QNetworkReply::NetworkError error);
    void slotExpandFinished(QNetworkReply *reply);

private:
    ScamExpandUrlJobPrivate *const d;
};
}
