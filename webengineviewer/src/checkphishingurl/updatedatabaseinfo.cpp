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

#include "updatedatabaseinfo.h"
#include "webengineviewer_debug.h"

using namespace WebEngineViewer;

UpdateDataBaseInfo::UpdateDataBaseInfo()
    : responseType(Unknown)
{

}

bool UpdateDataBaseInfo::isValid() const
{
    return (responseType != Unknown);
}

void UpdateDataBaseInfo::clear()
{
    additionList.clear();
    removalList.clear();
    minimumWaitDuration.clear();
    threatType.clear();
    threatEntryType.clear();
    responseType = UpdateDataBaseInfo::Unknown;
    platformType.clear();
    newClientState.clear();
    sha256.clear();
}

bool UpdateDataBaseInfo::operator==(const UpdateDataBaseInfo &other) const
{
    const bool val = (additionList == other.additionList) &&
                     (removalList == other.removalList) &&
                     (minimumWaitDuration == other.minimumWaitDuration) &&
                     (threatType == other.threatType) &&
                     (threatEntryType == other.threatEntryType) &&
                     (responseType == other.responseType) &&
                     (platformType == other.platformType) &&
                     (newClientState == other.newClientState) &&
                     (sha256 == other.sha256);
    if (!val) {
        qCWarning(WEBENGINEVIEWER_LOG) << " sha256 " << sha256 << " other.sha256 " << other.sha256;
        qCWarning(WEBENGINEVIEWER_LOG) << " minimumWaitDuration " << minimumWaitDuration << " other.minimumWaitDuration " << other.minimumWaitDuration;
        qCWarning(WEBENGINEVIEWER_LOG) << " threatType " << threatType << " other.threatType " << other.threatType;
        qCWarning(WEBENGINEVIEWER_LOG) << " threatEntryType " << threatEntryType << " other.threatEntryType " << other.threatEntryType;
        qCWarning(WEBENGINEVIEWER_LOG) << " responseType " << responseType << " other.responseType " << other.responseType;
        qCWarning(WEBENGINEVIEWER_LOG) << " platformType " << platformType << " other.platformType " << other.platformType;
        qCWarning(WEBENGINEVIEWER_LOG) << " newClientState " << newClientState << " other.newClientState " << other.newClientState;
        qCWarning(WEBENGINEVIEWER_LOG) << " threatType " << threatType << " other.threatType " << other.threatType;
        qCWarning(WEBENGINEVIEWER_LOG) << " removalList" << removalList.count() << " other.removalList " << other.removalList.count();
        qCWarning(WEBENGINEVIEWER_LOG) << " additionList" << additionList.count() << " other.additionList " << other.additionList.count();
    }
    return val;
}

Removal::Removal()
    : compressionType(UpdateDataBaseInfo::UnknownCompression)
{

}

bool Removal::operator==(const Removal &other) const
{
    bool value = (indexes == other.indexes) && (compressionType == other.compressionType) && (riceDeltaEncoding == other.riceDeltaEncoding);
    if (!value) {
        qCWarning(WEBENGINEVIEWER_LOG) << " indexes " << indexes << " other.indexes " << other.indexes;
        qCWarning(WEBENGINEVIEWER_LOG) << "compressionType " << compressionType << " other.compressionType " << other.compressionType;
    }
    return value;
}

bool Removal::isValid() const
{
    if (compressionType == UpdateDataBaseInfo::UnknownCompression) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Compression Type undefined";
        return false;
    }
    return !indexes.isEmpty();
}

Addition::Addition()
    : compressionType(UpdateDataBaseInfo::UnknownCompression),
      prefixSize(0)
{

}

bool Addition::isValid() const
{
    if (compressionType == UpdateDataBaseInfo::UnknownCompression) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Compression Type undefined";
        return false;
    }
    const bool hasCorrectPrefixSize = (prefixSize >= 4) && (prefixSize <= 32);
    if (!hasCorrectPrefixSize) {
        qCWarning(WEBENGINEVIEWER_LOG) << "Prefix size is not correct";
        return false;
    }
    if ((hashString.size() % static_cast<int>(prefixSize)) != 0) {
        qDebug()<< " hashString.size() "<< hashString.size() << "prefixSize "<<prefixSize;
        qCWarning(WEBENGINEVIEWER_LOG) << "it's not a correct hash value";
        return false;
    }
    return !hashString.isEmpty();
}

bool Addition::operator==(const Addition &other) const
{
    bool value = (hashString == other.hashString) &&
                 (prefixSize == other.prefixSize) &&
                 (compressionType == other.compressionType) &&
                 (riceDeltaEncoding == other.riceDeltaEncoding);
    if (!value) {
        qCWarning(WEBENGINEVIEWER_LOG) << "hashString " << hashString << " other.hashString " << other.hashString;
        qCWarning(WEBENGINEVIEWER_LOG) << "prefixSize " << prefixSize << " other.prefixSize " << other.prefixSize;
        qCWarning(WEBENGINEVIEWER_LOG) << "compressionType " << compressionType << " other.compressionType " << other.compressionType;
    }
    return value;
}

bool Addition::lessThan(const Addition &s1, const Addition &s2)
{
    return s1.hashString < s2.hashString;
}

RiceDeltaEncoding::RiceDeltaEncoding()
    : riceParameter(0),
      numberEntries(0)
{

}

bool RiceDeltaEncoding::operator==(const RiceDeltaEncoding &other) const
{
    return (firstValue == other.firstValue) &&
           (encodingData == other.encodingData) &&
           (riceParameter == other.riceParameter) &&
           (numberEntries == other.numberEntries);
}

bool RiceDeltaEncoding::isValid() const
{
    if (!firstValue.isEmpty() && !encodingData.isEmpty() &&((riceParameter >= 2 && riceParameter <= 28) || (riceParameter == 0 && numberEntries == 0))) {
        return true;
    }
    return false;
}
