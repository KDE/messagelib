/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::CreatePhishingUrlDataBaseJob
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/CreatePhishingUrlDataBaseJob
 *
 * \brief The CreatePhishingUrlDataBaseJob class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT CreatePhishingUrlDataBaseJob : public QObject
{
    Q_OBJECT
public:
    enum DataBaseDownloadType : uint8_t {
        FullDataBase = 0,
        UpdateDataBase = 1,
    };

    enum DataBaseDownloadResult : uint8_t {
        InvalidData = 0,
        ValidData = 1,
        UnknownError = 2,
        BrokenNetwork = 3,
    };

    enum ContraintsCompressionType : uint8_t {
        RawCompression = 0,
        RiceCompression = 1,
    };

    explicit CreatePhishingUrlDataBaseJob(QObject *parent = nullptr);
    /*! Destroys the CreatePhishingUrlDataBaseJob object. */
    ~CreatePhishingUrlDataBaseJob() override;

    /*! Starts the database download job. */
    void start();

    /*! Sets the current database state. */
    void setDataBaseState(const QString &value);

    /*! Sets the type of database download needed. */
    void setDataBaseDownloadNeeded(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType type);

    /*! Returns the JSON request payload. */
    [[nodiscard]] QByteArray jsonRequest() const;

    /*! Parses the result from the server. */
    void parseResult(const QByteArray &value);

    /*! Sets the compression type for constraints. */
    void setContraintsCompressionType(CreatePhishingUrlDataBaseJob::ContraintsCompressionType type);

Q_SIGNALS:
    /*! Emitted when the database creation is finished. */
    void finished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    /*! Emitted for debugging the JSON result. */
    void debugJsonResult(const QByteArray &ba);
    /*! Emitted for debugging JSON messages. */
    void debugJson(const QByteArray &ba);

private:
    WEBENGINEVIEWER_NO_EXPORT void slotDownloadDataBaseFinished(QNetworkReply *reply);
    WEBENGINEVIEWER_NO_EXPORT void slotError(QNetworkReply::NetworkError error);

    std::unique_ptr<CreatePhishingUrlDataBaseJobPrivate> const d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)
Q_DECLARE_METATYPE(WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType)
