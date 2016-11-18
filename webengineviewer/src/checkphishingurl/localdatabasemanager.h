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

#ifndef LOCALDATABASEMANAGER_H
#define LOCALDATABASEMANAGER_H

#include <QObject>
#include "webengineviewer_export.h"
#include <QSqlDatabase>
#include <QUrl>
#include "createphishingurldatabasejob.h"
class QSqlError;
namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT LocalDataBaseManager : public QObject
{
    Q_OBJECT
public:
    enum UrlStatus {
        Unknown = 0,
        UrlOk = 1,
        Malware = 2
    };

    explicit LocalDataBaseManager(QObject *parent = Q_NULLPTR);
    ~LocalDataBaseManager();

    static LocalDataBaseManager *self();

    void checkUrl(const QUrl &url);

    void start();

    void closeDataBaseAndDeleteIt();

Q_SIGNALS:
    void checkUrlFinished(const QUrl &url, WebEngineViewer::LocalDataBaseManager::UrlStatus status);
private:

    void readConfig();
    void saveConfig();

    void slotDownloadFullDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    QSqlError initDb();
    bool initializeDataBase();
    bool createTable();
    QSqlDatabase mDataBase;
    bool mDataBaseOk;
};
}

#endif // LOCALDATABASEMANAGER_H
