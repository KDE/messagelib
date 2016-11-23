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

#include "createdatabasefilejob.h"
#include "webengineviewer_debug.h"

using namespace WebEngineViewer;

CreateDatabaseFileJob::CreateDatabaseFileJob(QObject *parent)
    : QObject(parent)
{

}

CreateDatabaseFileJob::~CreateDatabaseFileJob()
{

}

bool CreateDatabaseFileJob::canStart() const
{
    return !mFileName.isEmpty() && mInfoDataBase.isValid();
}

void CreateDatabaseFileJob::setUpdateDataBaseInfo(const UpdateDataBaseInfo &infoDataBase)
{
    mInfoDataBase = infoDataBase;
}

void CreateDatabaseFileJob::start()
{
    if (!canStart()) {
        Q_EMIT finished(false);
        deleteLater();
    } else {
        createBinaryFile();
    }
}

void CreateDatabaseFileJob::createBinaryFile()
{
    switch (mInfoDataBase.responseType) {
    case UpdateDataBaseInfo::Unknown:
        //TODO error here
        break;
    case UpdateDataBaseInfo::FullUpdate:
    case UpdateDataBaseInfo::PartialUpdate:
        generateFile((mInfoDataBase.responseType == UpdateDataBaseInfo::FullUpdate));
        break;
    }
    deleteLater();
}

void CreateDatabaseFileJob::generateFile(bool fullUpdate)
{
    if (fullUpdate) {
        if (!QFile(mFileName).remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file "<< mFileName;
        }
        mFile.setFileName(mFileName);
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file "<< mFileName;
            //TODO ? assert ?
            Q_EMIT finished(false);
            return;
        }
        addElementToDataBase(mInfoDataBase.additionList);
    } else {
        removeElementFromDataBase(mInfoDataBase.removalList);
        addElementToDataBase(mInfoDataBase.additionList);
    }
}

void CreateDatabaseFileJob::setFileName(const QString &filename)
{
    mFileName = filename;
}

void CreateDatabaseFileJob::removeElementFromDataBase(const QVector<Removal> &removalList)
{
    Q_FOREACH (const Removal &removeItem, removalList) {
        Q_FOREACH (int id, removeItem.indexes) {
            //TODO
        }
    }
}

void CreateDatabaseFileJob::addElementToDataBase(const QVector<Addition> &additionList)
{
    Q_FOREACH (const Addition &add, additionList) {
        //qDebug() << " add.size" << add.prefixSize;
        //qDebug() << " add.hash" << QByteArray::fromBase64(add.hashString).size();
        const QByteArray uncompressed = QByteArray::fromBase64(add.hashString);
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;
            //qDebug() << "m " << m << " m.size" << m.size();
            //TODO add in database
        }
    }
}
