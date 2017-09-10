/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#include "localdatabasefile.h"
#include "checkphishingurlutil.h"
#include "updatedatabaseinfo.h"
#include "webengineviewer_debug.h"
#include <QElapsedTimer>
#include <QFileInfo>
#include <QDebug>

using namespace WebEngineViewer;

WEBENGINEVIEWER_EXPORT int webengineview_LocalDataBaseFile = 1000 * 60 * 60;

class WebEngineViewer::LocalDataBaseFilePrivate
{
public:
    LocalDataBaseFilePrivate(const QString &filename, LocalDataBaseFile *qq)
        : mFile(filename)
        , mData(nullptr)
        , q(qq)
        , mValid(false)
    {
    }

    bool load();
    bool reload();
    void close();

    QElapsedTimer mLastCheck;
    QFile mFile;
    uchar *mData;
    QDateTime mMtime;
    LocalDataBaseFile *q;
    bool mValid;
};

bool LocalDataBaseFilePrivate::load()
{
    if (!mFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    mData = mFile.map(0, mFile.size());
    if (mData) {
        const int major = q->getUint16(0);
        const int minor = q->getUint16(2);

        //Check version in binary file. => version value == 1.0 for the moment see CheckPhishingUrlUtil
        mValid = (major == WebEngineViewer::CheckPhishingUrlUtil::majorVersion()
                  && minor == WebEngineViewer::CheckPhishingUrlUtil::minorVersion());
    }
    mMtime = QFileInfo(mFile).lastModified();
    return mValid;
}

void LocalDataBaseFilePrivate::close()
{
    mValid = false;
    if (mFile.isOpen()) {
        mFile.close();
    }
    mData = nullptr;
}

bool LocalDataBaseFilePrivate::reload()
{
    qCDebug(WEBENGINEVIEWER_LOG) << "Reload File" << mFile.fileName();
    close();
    return load();
}

LocalDataBaseFile::LocalDataBaseFile(const QString &filename)
    : d(new WebEngineViewer::LocalDataBaseFilePrivate(filename, this))
{
    d->load();
}

LocalDataBaseFile::~LocalDataBaseFile()
{
    delete d;
}

void LocalDataBaseFile::close()
{
    d->close();
}

bool LocalDataBaseFile::isValid() const
{
    return d->mValid;
}

quint16 LocalDataBaseFile::getUint16(int offset) const
{
    return *reinterpret_cast<quint16 *>(d->mData + offset);
}

quint32 LocalDataBaseFile::getUint32(int offset) const
{
    return *reinterpret_cast<quint32 *>(d->mData + offset);
}

quint64 LocalDataBaseFile::getUint64(int offset) const
{
    return *reinterpret_cast<quint64 *>(d->mData + offset);
}

const char *LocalDataBaseFile::getCharStar(int offset) const
{
    return reinterpret_cast<const char *>(d->mData + offset);
}

bool LocalDataBaseFile::reload()
{
    return d->reload();
}

QByteArray LocalDataBaseFile::searchHash(const QByteArray &hashToSearch)
{
    /* database file has:
     * - one getUint16 => major
     * - one getUint16 => minor
     * - one getUint64 => number of item
     * => 12
     */
    int posListOffset = 12;
    const int numHash = getUint64(4);
    int begin = 0;
    int end = numHash - 1;
    QByteArray previousValue;
    if (end > 0) {
        QByteArray currentValue;
        do {
            previousValue = currentValue;
            const int medium = (begin + end) / 2;
            const int off = posListOffset + 8 * medium;
            const int hashOffset = getUint64(off);
            const char *hashCharStar = getCharStar(hashOffset);
            const int cmp = qstrcmp(hashCharStar, hashToSearch.constData());
            currentValue = QByteArray(hashCharStar);
            qCWarning(WEBENGINEVIEWER_LOG) << "search " << hashToSearch.toBase64() << " begin " << begin << " end " << end << " hashCharStar" << currentValue.toBase64();
            if (end == begin) {
                return currentValue;
            }
            if (cmp < 0) {
                begin = medium + 1;
            } else if (cmp > 0) {
                end = medium - 1;
            } else {
                return currentValue;
            }
        } while (begin <= end);
    }
    return previousValue;
}

bool LocalDataBaseFile::shouldCheck() const
{
    // 1 hours
    if (d->mLastCheck.isValid() && d->mLastCheck.elapsed() < webengineview_LocalDataBaseFile) {
        return false;
    }
    d->mLastCheck.start();
    return true;
}

bool LocalDataBaseFile::checkFileChanged()
{
    bool somethingChanged = false;
    QFileInfo fileInfo(d->mFile);
    if (!fileInfo.exists() || fileInfo.lastModified() > d->mMtime) {
        // But the user could use rm -rf :-)
        reload(); // will mark itself as invalid on failure
        somethingChanged = true;
    }
    if (somethingChanged) {
        return isValid();
    }
    return somethingChanged;
}

QVector<WebEngineViewer::Addition> LocalDataBaseFile::extractAllInfo() const
{
    QVector<WebEngineViewer::Addition> lst;
    quint64 numberOfElement = getUint64(4);
    int index = 12; // quint16 major + quint16 minor + quint64 number of element
    for (quint64 i = 0; i < numberOfElement; ++i) {
        const quint64 value = getUint64(index);
        Addition tmp;
        tmp.hashString = QByteArray(getCharStar(value));
        tmp.prefixSize = tmp.hashString.size();
        tmp.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;
        index += 8; //next index based on quint64
        lst.append(tmp);
    }
    return lst;
}

bool LocalDataBaseFile::fileExists() const
{
    QFileInfo fileInfo(d->mFile);
    return fileInfo.exists();
}
