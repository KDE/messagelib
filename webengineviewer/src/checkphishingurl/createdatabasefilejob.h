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

#ifndef CREATEDATABASEFILEJOB_H
#define CREATEDATABASEFILEJOB_H

#include <QObject>
#include "webengineviewer_export.h"
#include "updatedatabaseinfo.h"
#include <QFile>

namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT CreateDatabaseFileJob : public QObject
{
    Q_OBJECT
public:
    explicit CreateDatabaseFileJob(QObject *parent = Q_NULLPTR);
    ~CreateDatabaseFileJob();
    void start();
    void setFileName(const QString &filename);
    bool canStart() const;
    void setUpdateDataBaseInfo(const WebEngineViewer::UpdateDataBaseInfo &infoDataBase);

Q_SIGNALS:
    void finished(bool success);

private:
    void removeElementFromDataBase(const QVector<Removal> &removalList, QVector<Addition> &oldDataBaseAddition);
    void createBinaryFile();
    void generateFile(bool fullUpdate);
    void createFileFromFullUpdate(const QVector<Addition> &additionList, const QByteArray &sha256);
    WebEngineViewer::UpdateDataBaseInfo mInfoDataBase;
    QString mFileName;
    QFile mFile;
};
}

#endif // CREATEDATABASEFILEJOB_H
