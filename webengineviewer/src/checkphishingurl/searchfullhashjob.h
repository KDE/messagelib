/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#ifndef SEARCHFULLHASHJOB_H
#define SEARCHFULLHASHJOB_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include "webengineviewer_export.h"
#include "checkphishingurlutil.h"
namespace WebEngineViewer {
class SearchFullHashJobPrivate;
/* https://developers.google.com/safe-browsing/v4/update-api */
class WEBENGINEVIEWER_EXPORT SearchFullHashJob : public QObject
{
    Q_OBJECT
public:
    explicit SearchFullHashJob(QObject *parent = nullptr);
    ~SearchFullHashJob();

    void start();
    bool canStart() const;

    void setDatabaseState(const QStringList &hash);
    void setSearchFullHashForUrl(const QUrl &url);

    QByteArray jsonRequest() const;
    void parse(const QByteArray &replyStr);

    void setSearchHashs(const QHash<QByteArray, QByteArray> &hash);
Q_SIGNALS:
    void result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url);
    void debugJson(const QByteArray &ba);

private Q_SLOTS:
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    void slotError(QNetworkReply::NetworkError error);
    void slotCheckUrlFinished(QNetworkReply *reply);
private:
    Q_DISABLE_COPY(SearchFullHashJob)
    SearchFullHashJobPrivate *const d;
};
}

#endif // SEARCHFULLHASHJOB_H
