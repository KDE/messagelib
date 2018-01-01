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

#ifndef CREATEPHISHINGURLDATABASEJOB_H
#define CREATEPHISHINGURLDATABASEJOB_H

#include <QObject>
#include <QSslError>
#include <QNetworkReply>

#include "webengineviewer_export.h"
namespace WebEngineViewer {
struct UpdateDataBaseInfo;
class CreatePhishingUrlDataBaseJobPrivate;
/* https://developers.google.com/safe-browsing/v4/update-api */
class WEBENGINEVIEWER_EXPORT CreatePhishingUrlDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum DataBaseDownloadType {
        FullDataBase = 0,
        UpdateDataBase = 1
    };

    enum DataBaseDownloadResult {
        InvalidData = 0,
        ValidData = 1,
        UnknownError = 2,
        BrokenNetwork = 3
    };

    enum ContraintsCompressionType {
        RawCompression = 0,
        RiceCompression = 1
    };

    explicit CreatePhishingUrlDataBaseJob(QObject *parent = nullptr);
    ~CreatePhishingUrlDataBaseJob();

    void start();

    void setDataBaseState(const QString &value);

    void setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType type);

    QByteArray jsonRequest() const;

    void parseResult(const QByteArray &value);

    void setContraintsCompressionType(CreatePhishingUrlDataBaseJob::ContraintsCompressionType type);

Q_SIGNALS:
    void finished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    void debugJsonResult(const QByteArray &ba);
    void debugJson(const QByteArray &ba);

private:
    void slotDownloadDataBaseFinished(QNetworkReply *reply);
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    void slotError(QNetworkReply::NetworkError error);

    CreatePhishingUrlDataBaseJobPrivate *const d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType)
#endif // CREATEPHISHINGURLDATABASEJOB_H
