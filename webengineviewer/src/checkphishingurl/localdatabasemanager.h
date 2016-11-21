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
#include <QUrl>
#include "createphishingurldatabasejob.h"
namespace WebEngineViewer
{
class LocalDataBaseManagerPrivate;
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

    void initialize();

    void closeDataBaseAndDeleteIt();


Q_SIGNALS:
    void checkUrlFinished(const QUrl &url, WebEngineViewer::LocalDataBaseManager::UrlStatus status);

protected:
    void setDownloadProgress(bool downloadProgress);
    virtual void downloadFullDataBase();
    virtual void downloadPartialDataBase();

private:
    void readConfig();
    void saveConfig();

    void fullUpdateDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);
    void slotDownloadDataBaseFinished(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase, WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult status);
    void partialUpdateDataBase(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);
    void addElementToDataBase(const QVector<Addition> &additionList);
    void removeElementFromDataBase(const QVector<Removal> &removalList);
    void slotCheckDataBase();

    bool initializeDataBase();
    QByteArray createHash(const QUrl &url);
    void checkDataBase();
    bool malwareFound(const QByteArray &hash);


    LocalDataBaseManagerPrivate *const d;
};
}

#endif // LOCALDATABASEMANAGER_H
