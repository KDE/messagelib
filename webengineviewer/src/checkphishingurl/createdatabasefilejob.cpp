/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "createdatabasefilejob.h"
#include "checkphishingurlutil.h"
#include "localdatabasefile.h"
#include "riceencodingdecoder.h"
#include "webengineviewer_debug.h"

#include <QCryptographicHash>
#include <QFile>

using namespace WebEngineViewer;

class WebEngineViewer::CreateDatabaseFileJobPrivate
{
public:
    explicit CreateDatabaseFileJobPrivate(CreateDatabaseFileJob *qq)
        : q(qq)
    {
    }

    void createFileFromFullUpdate(const QList<Addition> &additionList);
    void removeElementFromDataBase(const QList<Removal> &removalList, QList<Addition> &oldDataBaseAddition);
    void createBinaryFile();
    void generateFile(bool fullUpdate);
    WebEngineViewer::UpdateDataBaseInfo mInfoDataBase;
    QString mFileName;
    QFile mFile;
    CreateDatabaseFileJob *const q;
};

void CreateDatabaseFileJobPrivate::createFileFromFullUpdate(const QList<Addition> &additionList)
{
    // 1 add version number
    const quint16 major = WebEngineViewer::CheckPhishingUrlUtil::majorVersion();
    const quint16 minor = WebEngineViewer::CheckPhishingUrlUtil::minorVersion();

    qint64 hashStartPosition = mFile.write(reinterpret_cast<const char *>(&major), sizeof(major));
    hashStartPosition += mFile.write(reinterpret_cast<const char *>(&minor), sizeof(minor));

    // 2 add number of items
    QList<Addition> itemToStore;
    for (const Addition &add : additionList) {
        switch (add.compressionType) {
        case UpdateDataBaseInfo::RawCompression: {
            // qCWarning(WEBENGINEVIEWER_LOG) << " add.size" << add.prefixSize;
            const QByteArray uncompressed = add.hashString;
            for (int i = 0; i < uncompressed.size();) {
                const QByteArray m = uncompressed.mid(i, add.prefixSize);
                i += add.prefixSize;

                Addition tmp;
                tmp.hashString = m;
                tmp.prefixSize = add.prefixSize;
                itemToStore << tmp;

                // We store index as 8 octets.
                hashStartPosition += 8;
                if (m.size() != add.prefixSize) {
                    qCWarning(WEBENGINEVIEWER_LOG) << "hash string: " << m << " hash string size: " << m.size();
                }
            }
            break;
        }
        case UpdateDataBaseInfo::RiceCompression: {
            // TODO
            qCWarning(WEBENGINEVIEWER_LOG) << "Rice compression still not implemented";
            const QList<quint32> listRice = WebEngineViewer::RiceEncodingDecoder::decodeRiceHashesDelta(add.riceDeltaEncoding);
            qDebug() << " listRice" << listRice;
            break;
        }
        case UpdateDataBaseInfo::UnknownCompression:
            qCWarning(WEBENGINEVIEWER_LOG) << "Unknown compression type in addition element";
            break;
        }
    }
    const quint64 numberOfElement = itemToStore.count();
    hashStartPosition += mFile.write(reinterpret_cast<const char *>(&numberOfElement), sizeof(numberOfElement));
    // 3 add index of items

    // Order it first
    std::sort(itemToStore.begin(), itemToStore.end(), Addition::lessThan);

    quint64 tmpPos = hashStartPosition;

    for (const Addition &add : std::as_const(itemToStore)) {
        mFile.write(reinterpret_cast<const char *>(&tmpPos), sizeof(tmpPos));
        tmpPos += add.prefixSize + 1; // We add +1 as we store '\0'
    }

    // 4 add items
    QByteArray newSsha256;
    for (const Addition &add : std::as_const(itemToStore)) {
        const QByteArray storedBa = add.hashString + '\0';
        mFile.write(reinterpret_cast<const char *>(storedBa.constData()), storedBa.size());
        newSsha256 += add.hashString;
    }
    mFile.close();
    // Verify hash with sha256
    const QByteArray newSsha256Value = QCryptographicHash::hash(newSsha256, QCryptographicHash::Sha256);

    const bool checkSumCorrect = (mInfoDataBase.sha256 == newSsha256Value.toBase64());
    if (!checkSumCorrect) {
        qCWarning(WEBENGINEVIEWER_LOG) << " newSsha256Value different from sha256 : " << newSsha256Value.toBase64() << " from server " << mInfoDataBase.sha256;
    }
    Q_EMIT q->finished(checkSumCorrect, mInfoDataBase.newClientState, mInfoDataBase.minimumWaitDuration);
}

void CreateDatabaseFileJobPrivate::generateFile(bool fullUpdate)
{
    qCDebug(WEBENGINEVIEWER_LOG) << " void CreateDatabaseFileJobPrivate::generateFile(bool fullUpdate)" << fullUpdate;
    mFile.setFileName(mFileName);
    if (fullUpdate) {
        if (mFile.exists() && !mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file " << mFileName;
            Q_EMIT q->finished(false, QString(), QString());
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT q->finished(false, QString(), QString());
            return;
        }
        createFileFromFullUpdate(mInfoDataBase.additionList);
    } else {
        WebEngineViewer::LocalDataBaseFile localeFile(mFileName);
        if (!localeFile.fileExists()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to create partial update as file doesn't exist";
            Q_EMIT q->finished(false, QString(), QString());
            return;
        }
        // Read Element from database.
        QList<Addition> oldDataBaseAddition = localeFile.extractAllInfo();

        removeElementFromDataBase(mInfoDataBase.removalList, oldDataBaseAddition);
        QList<Addition> additionList = mInfoDataBase.additionList; // Add value found in database
        oldDataBaseAddition += additionList;

        // Close file
        localeFile.close();

        if (!mFile.remove()) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove database file " << mFileName;
            Q_EMIT q->finished(false, QString(), QString());
            return;
        }
        if (!mFile.open(QIODevice::WriteOnly)) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to open database file " << mFileName;
            Q_EMIT q->finished(false, QString(), QString());
            return;
        }
        createFileFromFullUpdate(oldDataBaseAddition);
    }
}

void CreateDatabaseFileJobPrivate::removeElementFromDataBase(const QList<Removal> &removalList, QList<Addition> &oldDataBaseAddition)
{
    QList<quint32> indexToRemove;
    for (const Removal &removeItem : removalList) {
        switch (removeItem.compressionType) {
        case UpdateDataBaseInfo::RawCompression:
            for (int id : std::as_const(removeItem.indexes)) {
                indexToRemove << id;
            }
            break;
        case UpdateDataBaseInfo::RiceCompression:
            indexToRemove = WebEngineViewer::RiceEncodingDecoder::decodeRiceIndiceDelta(removeItem.riceDeltaEncoding);
            break;
        case UpdateDataBaseInfo::UnknownCompression:
            qCWarning(WEBENGINEVIEWER_LOG) << " Unknown compression type defined in removal elements. It's a bug!";
            break;
        }
    }

    std::sort(indexToRemove.begin(), indexToRemove.end());
    for (int i = (indexToRemove.count() - 1); i >= 0; --i) {
        oldDataBaseAddition.remove(indexToRemove.at(i));
    }
}

void CreateDatabaseFileJobPrivate::createBinaryFile()
{
    switch (mInfoDataBase.responseType) {
    case UpdateDataBaseInfo::Unknown:
        qCWarning(WEBENGINEVIEWER_LOG) << " Response Type of database info is \"unknown\". It's a bug!";
        break;
    case UpdateDataBaseInfo::FullUpdate:
    case UpdateDataBaseInfo::PartialUpdate:
        generateFile((mInfoDataBase.responseType == UpdateDataBaseInfo::FullUpdate));
        break;
    }
    q->deleteLater();
}

CreateDatabaseFileJob::CreateDatabaseFileJob(QObject *parent)
    : QObject(parent)
    , d(new WebEngineViewer::CreateDatabaseFileJobPrivate(this))
{
}

CreateDatabaseFileJob::~CreateDatabaseFileJob() = default;

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
        Q_EMIT finished(false, QString(), QString());
        deleteLater();
    } else {
        d->createBinaryFile();
    }
}

void CreateDatabaseFileJob::setFileName(const QString &filename)
{
    d->mFileName = filename;
}

#include "moc_createdatabasefilejob.cpp"
