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


#ifndef CREATEPHISHINGURLDATABASEJOB_H
#define CREATEPHISHINGURLDATABASEJOB_H

#include <QObject>
#include <QSslError>

#include "webengineviewer_export.h"
class QNetworkAccessManager;
class QNetworkReply;
namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT CreatePhishingUrlDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum DataBaseDownload {
        FullDataBase = 0,
        UpdateDataBase = 1
    };

    explicit CreatePhishingUrlDataBaseJob(QObject *parent = Q_NULLPTR);
    ~CreatePhishingUrlDataBaseJob();

    void start();

    void setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload type);

private:
    void slotCheckUrlFinished(QNetworkReply *reply);
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);

    DataBaseDownload mDataBaseDownloadNeeded;
    QNetworkAccessManager *mNetworkAccessManager;
};
}
#endif // CREATEPHISHINGURLDATABASEJOB_H
