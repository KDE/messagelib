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
#include "localdatabasefile.h"
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
    mFile.setFileName(mFileName);
    if (fullUpdate) {
        if (mFile.exists() && !mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file " << mFileName;
            Q_EMIT finished(false);
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT finished(false);
            return;
        }
        createFileFromFullUpdate(mInfoDataBase.additionList, mInfoDataBase.sha256);
    } else {
        WebEngineViewer::LocalDataBaseFile localeFile(mFileName);
        if (!localeFile.fileExists()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to create partial update as file doesn't exist";
            Q_EMIT finished(false);
            return;
        }
        //Read Element from database.
        QVector<Addition> oldDataBaseAddition = localeFile.extractAllInfo();

        removeElementFromDataBase(mInfoDataBase.removalList, oldDataBaseAddition);
        QVector<Addition> additionList = mInfoDataBase.additionList; // Add value found in database
        oldDataBaseAddition += additionList;

        //Close file
        localeFile.close();

        if (!mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file " << mFileName;
            Q_EMIT finished(false);
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT finished(false);
            return;
        }
        createFileFromFullUpdate(oldDataBaseAddition, mInfoDataBase.sha256);
    }
}

void CreateDatabaseFileJob::setFileName(const QString &filename)
{
    mFileName = filename;
}

void CreateDatabaseFileJob::removeElementFromDataBase(const QVector<Removal> &removalList, QVector<Addition> &oldDataBaseAddition)
{
    //qDebug() << " oldDataBaseAddition.count() BEFORE : " << oldDataBaseAddition.count();
    QList<int> indexToRemove;
    Q_FOREACH (const Removal &removeItem, removalList) {
        Q_FOREACH (int id, removeItem.indexes) {
            indexToRemove << id;
        }
    }
    //qDebug() << "indexToRemove.count()" << indexToRemove.count() << " indexToRemove "<<indexToRemove;
    qSort(indexToRemove);
    for (int i = (indexToRemove.count() - 1); i >= 0; --i) {
        oldDataBaseAddition.remove(indexToRemove.at(i));
        //qDebug() << indexToRemove.at(i);
    }
    //qDebug() << " oldDataBaseAddition.count() AFTER : " << oldDataBaseAddition.count();
}

void CreateDatabaseFileJob::createFileFromFullUpdate(const QVector<Addition> &additionList, const QByteArray &sha256)
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
        //qCWarning(WEBENGINEVIEWER_LOG) << " add.size" << add.prefixSize;
        const QByteArray uncompressed = add.hashString;
        for (int i = 0; i < uncompressed.size();) {
            const QByteArray m = uncompressed.mid(i, add.prefixSize);
            i += add.prefixSize;

            Addition tmp;
            tmp.hashString = m;
            tmp.prefixSize = add.prefixSize;
            itemToStore << tmp;

            //We store index as 8 octets.
            hashStartPosition += 8;
            if (m.size() != add.prefixSize) {
                qCWarning(WEBENGINEVIEWER_LOG) << "m " << m << " m.size" << m.size();
            }
        }
    }
    const quint64 numberOfElement = itemToStore.count();
    hashStartPosition += mFile.write(reinterpret_cast<const char *>(&numberOfElement), sizeof(numberOfElement));
    //3 add index of items

    //Order it first
    std::sort(itemToStore.begin(), itemToStore.end(), Addition::lessThan);

    quint64 tmpPos = hashStartPosition;

    Q_FOREACH (const Addition &add, itemToStore) {
        mFile.write(reinterpret_cast<const char *>(&tmpPos), sizeof(tmpPos));
        tmpPos += add.prefixSize + 1; //We add +1 as we store '\0'
    }
    //TODO verify position.

    //4 add items
    QByteArray newSsha256;
    Q_FOREACH (const Addition &add, itemToStore) {
        const QByteArray storedBa = add.hashString + '\0';
        mFile.write(reinterpret_cast<const char *>(storedBa.constData()), storedBa.size());
        newSsha256 += add.hashString;
    }
    mFile.close();
    //Verify hash with sha256

    const QByteArray newSsha256Value = QCryptographicHash::hash(newSsha256, QCryptographicHash::Sha256);
    //qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value"<<newSsha256Value;
    //qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value"<<newSsha256Value.toBase64();
    //qCWarning(WEBENGINEVIEWER_LOG) << " sha256 " << sha256;

    const bool checkSumCorrect = (sha256 == newSsha256Value.toBase64());
    if (!checkSumCorrect) {
        qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value different from sha256 : " << newSsha256Value.toBase64() << " from server " << sha256;
    }
    finished(checkSumCorrect);
}
