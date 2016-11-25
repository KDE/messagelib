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
#include "checkphishingurlutil.h"
#include "webengineviewer_debug.h"
#include <QFileInfo>
#include <QCryptographicHash>

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
        mFile.setFileName(mFileName);
        if (mFile.exists() && !mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file "<< mFileName;
            Q_EMIT finished(false);
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file "<< mFileName;
            //TODO ? assert ?
            Q_EMIT finished(false);
            return;
        }
        createFileFromFullUpdate(mInfoDataBase.additionList, mInfoDataBase.sha256);
    } else {
        removeElementFromDataBase(mInfoDataBase.removalList);
        //addElementToDataBase(mInfoDataBase.additionList);
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

void CreateDatabaseFileJob::createFileFromFullUpdate(const QVector<Addition> &additionList, const QString &sha256)
{
    //1 add version number
    const quint16 major = WebEngineViewer::CheckPhishingUrlUtil::majorVersion();
    const quint16 minor = WebEngineViewer::CheckPhishingUrlUtil::minorVersion();

    qint64 hashStartPosition = mFile.write(reinterpret_cast<const char *>(&major), sizeof(major));
    hashStartPosition += mFile.write(reinterpret_cast<const char *>(&minor), sizeof(minor));

    //2 add number of items
    QList<Addition> itemToStore;
    //TODO look at raw or rice! Decode it.
    Q_FOREACH (const Addition &add, additionList) {
        //qDebug() << " add.size" << add.prefixSize;
        const QByteArray uncompressed = add.hashString;
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;

            Addition tmp;
            tmp.hashString = m;
            tmp.prefixSize = add.prefixSize;
            itemToStore << tmp;

            hashStartPosition += tmp.prefixSize;
            if (m.size() != add.prefixSize) {
                qDebug() << "m " << m << " m.size" << m.size();
            }
        }
    }
    const qint64 numberOfElement = itemToStore.count();
    hashStartPosition += mFile.write(reinterpret_cast<const char *>(&numberOfElement), sizeof(numberOfElement));

    //3 add index of items
    qint64 tmpPos = hashStartPosition;
    Q_FOREACH (const Addition &add, itemToStore) {
        mFile.write(reinterpret_cast<const char *>(&tmpPos), sizeof(tmpPos));
        tmpPos += add.prefixSize;
    }
    //TODO verify position.

    //4 add items
    QByteArray newSsha256;
    std::sort(itemToStore.begin(), itemToStore.end(), Addition::lessThan);
    Q_FOREACH (const Addition &add, itemToStore) {
        QByteArray ba = add.hashString;
        mFile.write(reinterpret_cast<const char *>(ba.constData()), add.hashString.size());
        newSsha256 += ba;
        //qDebug() << " ba " << ba;
    }
    mFile.close();
    //Verify hash with sha256

    const QByteArray newSsha256Value = QCryptographicHash::hash(newSsha256, QCryptographicHash::Sha256);
    //qDebug()<<" newSsha256Value"<<newSsha256Value;
    //qDebug()<<" newSsha256Value"<<newSsha256Value.toBase64();
    //qDebug() << " sha256 " << sha256;

    bool checkSumCorrect = (sha256.toLatin1() == newSsha256Value.toBase64());
    if (!checkSumCorrect) {
        qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value different from sha256 : " << newSsha256Value.toBase64() << " from server " << sha256.toLatin1();
    }
    finished(checkSumCorrect);
}
