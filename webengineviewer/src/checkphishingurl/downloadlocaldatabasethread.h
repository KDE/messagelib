/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_private_export.h"
#include <QThread>
#include <WebEngineViewer/CreatePhishingUrlDataBaseJob>
namespace WebEngineViewer
{
class WEBENGINEVIEWER_TESTS_EXPORT DownloadLocalDatabaseThread : public QThread
{
    Q_OBJECT
public:
    /*! Constructs a DownloadLocalDatabaseThread object. */
    explicit DownloadLocalDatabaseThread(QObject *parent = nullptr);
    /*! Destroys the DownloadLocalDatabaseThread object. */
    ~DownloadLocalDatabaseThread() override;

    /*! Sets the current database state. */
    void setDataBaseState(const QString &value);

    /*! Sets the full path to the database file. */
    void setDatabaseFullPath(const QString &databaseFullPath);

Q_SIGNALS:
    /*! Emitted when database creation is finished. */
    void createDataBaseFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr);
    /*! Emitted when database creation fails. */
    void createDataBaseFailed();

protected:
    /*! Runs the thread. */
    void run() override;

private:
    WEBENGINEVIEWER_NO_EXPORT void installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);
    WEBENGINEVIEWER_NO_EXPORT void slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
                                                                WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    WEBENGINEVIEWER_NO_EXPORT void slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr);
    QString mCurrentDataBaseState;
    QString mDatabaseFullPath;
};
}
