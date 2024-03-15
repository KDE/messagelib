/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QNetworkReply>
#include <QObject>
#include <QSslError>

#include "webengineviewer_export.h"
namespace WebEngineViewer
{
struct UpdateDataBaseInfo;
class CreatePhishingUrlDataBaseJobPrivate;
/* https://developers.google.com/safe-browsing/v4/update-api */
/**
 * @brief The CreatePhishingUrlDataBaseJob class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CreatePhishingUrlDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum DataBaseDownloadType {
        FullDataBase = 0,
        UpdateDataBase = 1,
    };

    enum DataBaseDownloadResult {
        InvalidData = 0,
        ValidData = 1,
        UnknownError = 2,
        BrokenNetwork = 3,
    };

    enum ContraintsCompressionType {
        RawCompression = 0,
        RiceCompression = 1,
    };

    explicit CreatePhishingUrlDataBaseJob(QObject *parent = nullptr);
    ~CreatePhishingUrlDataBaseJob() override;

    void start();

    void setDataBaseState(const QString &value);

    void setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType type);

    [[nodiscard]] QByteArray jsonRequest() const;

    void parseResult(const QByteArray &value);

    void setContraintsCompressionType(CreatePhishingUrlDataBaseJob::ContraintsCompressionType type);

Q_SIGNALS:
    void finished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    void debugJsonResult(const QByteArray &ba);
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotDownloadDataBaseFinished(QNetworkReply *reply);
    WEBENGINEVIEWER_NO_EXPORT void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &error);
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);

    std::unique_ptr<CreatePhishingUrlDataBaseJobPrivate> const d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType)
