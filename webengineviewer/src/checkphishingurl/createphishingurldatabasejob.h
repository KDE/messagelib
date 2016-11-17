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
#include <QNetworkReply>

#include "webengineviewer_export.h"
class QNetworkAccessManager;
namespace WebEngineViewer
{
struct Addition {
    Addition()
        : prefixSize(0)
    {

    }
    bool isValid() const {
        return !hashString.isEmpty() && (prefixSize > 0);
    }
    bool operator==(const Addition &other) const;

    QByteArray hashString;
    int prefixSize;
};

struct Removal {
    Removal()
    {

    }

    bool operator==(const Removal &other) const;
    bool isValid() const {
        return !indexes.isEmpty();
    }
    QList<int> indexes;
};

struct UpdateDataBaseInfo {
    QVector<Addition> additionList;
    QVector<Removal> removalList;
    QString minimumWaitDuration;
    QString threatType;
    QString threatEntryType;
    QString responseType;
    QString platformType;
    QString newClientState;
    QString sha256;
    bool operator==(const UpdateDataBaseInfo &other) const;
};

/* https://developers.google.com/safe-browsing/v4/update-api */
class WEBENGINEVIEWER_EXPORT CreatePhishingUrlDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum DataBaseDownload {
        FullDataBase = 0,
        UpdateDataBase = 1
    };

    enum DataBaseDownloadResult {
        InvalidData = 0,
        ValidData = 1,
        UnknownError = 2,
        BrokenNetwork = 3
    };

    explicit CreatePhishingUrlDataBaseJob(QObject *parent = Q_NULLPTR);
    ~CreatePhishingUrlDataBaseJob();

    void start();

    void setDataBaseState(const QString &value);

    void setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload type);

    QByteArray jsonRequest() const;

    void setUseCompactJson(bool useCompactJson);
    void parseResult(const QByteArray &value);

Q_SIGNALS:
    void finished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    void debugJsonResult(const QByteArray &ba);
    void debugJson(const QByteArray &ba);

private:
    QString apiKey() const;
    void slotDownloadDataBaseFinished(QNetworkReply *reply);
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    void slotError(QNetworkReply::NetworkError error);
    QVector<Removal> parseRemovals(const QVariantList &lst);
    QVector<Addition> parseAdditions(const QVariantList &lst);

    QString mDataBaseState;
    DataBaseDownload mDataBaseDownloadNeeded;
    bool mUseCompactJson;
    QNetworkAccessManager *mNetworkAccessManager;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo)
Q_DECLARE_TYPEINFO(WebEngineViewer::Addition, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::Removal, Q_MOVABLE_TYPE);
#endif // CREATEPHISHINGURLDATABASEJOB_H
