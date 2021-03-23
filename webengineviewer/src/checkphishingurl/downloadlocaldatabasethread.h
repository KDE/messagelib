/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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
    explicit DownloadLocalDatabaseThread(QObject *parent = nullptr);
    ~DownloadLocalDatabaseThread() override;

    void setDataBaseState(const QString &value);

    void setDatabaseFullPath(const QString &databaseFullPath);

Q_SIGNALS:
    void createDataBaseFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr);
    void createDataBaseFailed();

protected:
    void run() override;

private:
    void installNewDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);
    void slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase,
                                      WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    void slotCreateDataBaseFileNameFinished(bool success, const QString &newClientState, const QString &minimumWaitDurationStr);
    QString mCurrentDataBaseState;
    QString mDatabaseFullPath;
};
}
