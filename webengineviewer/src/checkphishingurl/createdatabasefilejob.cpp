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

class WebEngineViewer::CreateDatabaseFileJobPrivate
{
public:
    CreateDatabaseFileJobPrivate(CreateDatabaseFileJob *qq)
        : q(qq)
    {

    }
    void createFileFromFullUpdate(const QVector<Addition> &additionList);
    void removeElementFromDataBase(const QVector<Removal> &removalList, QVector<Addition> &oldDataBaseAddition);
    void createBinaryFile();
    void generateFile(bool fullUpdate);
    WebEngineViewer::UpdateDataBaseInfo mInfoDataBase;
    QString mFileName;
    QFile mFile;
    CreateDatabaseFileJob *q;
};

void CreateDatabaseFileJobPrivate::createFileFromFullUpdate(const QVector<Addition> &additionList)
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
        switch(add.compressionType) {
        case UpdateDataBaseInfo::RawCompression: {
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
                    qCWarning(WEBENGINEVIEWER_LOG) << "hash string: " << m << " hash string size: " << m.size();
                }
            }
            break;
        }
        case UpdateDataBaseInfo::RiceCompression: {
            qCWarning(WEBENGINEVIEWER_LOG) << "Rice compression still not implemented";
            break;
        }
        case UpdateDataBaseInfo::UnknownCompression:
            qCWarning(WEBENGINEVIEWER_LOG) << "Unknow compression type in addition element";
            break;

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

    const bool checkSumCorrect = (mInfoDataBase.sha256 == newSsha256Value.toBase64());
    if (!checkSumCorrect) {
        qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value different from sha256 : " << newSsha256Value.toBase64() << " from server " << mInfoDataBase.sha256;
    }
    Q_EMIT q->finished(checkSumCorrect, mInfoDataBase.newClientState/*, mInfoDataBase.minimumWaitDuration*/);
}

void CreateDatabaseFileJobPrivate::generateFile(bool fullUpdate)
{
    qDebug() << " void CreateDatabaseFileJobPrivate::generateFile(bool fullUpdate)" << fullUpdate;
    mFile.setFileName(mFileName);
    if (fullUpdate) {
        if (mFile.exists() && !mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file " << mFileName;
            Q_EMIT q->finished(false, QString());
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT q->finished(false, QString());
            return;
        }
        createFileFromFullUpdate(mInfoDataBase.additionList);
    } else {
        WebEngineViewer::LocalDataBaseFile localeFile(mFileName);
        if (!localeFile.fileExists()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to create partial update as file doesn't exist";
            Q_EMIT q->finished(false, QString());
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
            Q_EMIT q->finished(false, QString());
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT q->finished(false, QString());
            return;
        }
        createFileFromFullUpdate(oldDataBaseAddition);
    }
}

void CreateDatabaseFileJobPrivate::removeElementFromDataBase(const QVector<Removal> &removalList, QVector<Addition> &oldDataBaseAddition)
{
    QList<int> indexToRemove;
    Q_FOREACH (const Removal &removeItem, removalList) {
        switch(removeItem.compressionType) {
        case UpdateDataBaseInfo::RawCompression: {
            Q_FOREACH (int id, removeItem.indexes) {
                indexToRemove << id;
            }
            break;
        }
        case UpdateDataBaseInfo::RiceCompression: {
            qCWarning(WEBENGINEVIEWER_LOG) << "Rice compression still not implemented in removal element";
            break;
        }
        case UpdateDataBaseInfo::UnknownCompression: {
            qCWarning(WEBENGINEVIEWER_LOG) << " UnknownCompression defined in removal elements. It's a bug";
            break;
        }
        }
    }

    qSort(indexToRemove);
    for (int i = (indexToRemove.count() - 1); i >= 0; --i) {
        oldDataBaseAddition.remove(indexToRemove.at(i));
    }
}

void CreateDatabaseFileJobPrivate::createBinaryFile()
{
    switch (mInfoDataBase.responseType) {
    case UpdateDataBaseInfo::Unknown:
        qCWarning(WEBENGINEVIEWER_LOG) << " Reponse Type of database info is \"unknow\". It's a bug!";
        break;
    case UpdateDataBaseInfo::FullUpdate:
    case UpdateDataBaseInfo::PartialUpdate:
        generateFile((mInfoDataBase.responseType == UpdateDataBaseInfo::FullUpdate));
        break;
    }
    q->deleteLater();
}

CreateDatabaseFileJob::CreateDatabaseFileJob(QObject *parent)
    : QObject(parent),
      d(new WebEngineViewer::CreateDatabaseFileJobPrivate(this))
{

}

CreateDatabaseFileJob::~CreateDatabaseFileJob()
{
    delete d;
}

bool CreateDatabaseFileJob::canStart() const
{
    return !d->mFileName.isEmpty() && d->mInfoDataBase.isValid();
}

void CreateDatabaseFileJob::setUpdateDataBaseInfo(const UpdateDataBaseInfo &infoDataBase)
{
    d->mInfoDataBase = infoDataBase;
}

void CreateDatabaseFileJob::start()
{
    if (!canStart()) {
        Q_EMIT finished(false, QString());
        deleteLater();
    } else {
        d->createBinaryFile();
    }
}

void CreateDatabaseFileJob::setFileName(const QString &filename)
{
    d->mFileName = filename;
}

