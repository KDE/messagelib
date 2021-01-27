/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "updatedatabaseinfo.h"
#include "webengineviewer_debug.h"

using namespace WebEngineViewer;

UpdateDataBaseInfo::UpdateDataBaseInfo()
{
}

bool UpdateDataBaseInfo::isValid() const
{
    return responseType != Unknown;
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
    const bool val = (additionList == other.additionList) && (removalList == other.removalList) && (minimumWaitDuration == other.minimumWaitDuration)
        && (threatType == other.threatType) && (threatEntryType == other.threatEntryType) && (responseType == other.responseType)
        && (platformType == other.platformType) && (newClientState == other.newClientState) && (sha256 == other.sha256);
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
    bool valid = false;
    switch (compressionType) {
    case UpdateDataBaseInfo::UnknownCompression:
        qCWarning(WEBENGINEVIEWER_LOG) << "Compression Type undefined";
        valid = false;
        break;
    case UpdateDataBaseInfo::RiceCompression:
        valid = riceDeltaEncoding.isValid();
        break;
    case UpdateDataBaseInfo::RawCompression:
        valid = !indexes.isEmpty();
        break;
    }
    return valid;
}

Addition::Addition()
    : compressionType(UpdateDataBaseInfo::UnknownCompression)
    , prefixSize(0)
{
}

bool Addition::isValid() const
{
    bool valid = false;
    switch (compressionType) {
    case UpdateDataBaseInfo::UnknownCompression:
        qCWarning(WEBENGINEVIEWER_LOG) << "Compression Type undefined";
        valid = false;
        break;
    case UpdateDataBaseInfo::RiceCompression:
        valid = riceDeltaEncoding.isValid();
        break;
    case UpdateDataBaseInfo::RawCompression:
        const bool hasCorrectPrefixSize = (prefixSize >= 4) && (prefixSize <= 32);
        if (!hasCorrectPrefixSize) {
            qCWarning(WEBENGINEVIEWER_LOG) << "Prefix size is not correct";
            valid = false;
        } else if ((hashString.size() % static_cast<int>(prefixSize)) != 0) {
            qDebug() << " hashString.size() " << hashString.size() << "prefixSize " << prefixSize;
            qCWarning(WEBENGINEVIEWER_LOG) << "it's not a correct hash value";
            valid = false;
        } else {
            valid = !hashString.isEmpty();
        }
        break;
    }
    return valid;
}

bool Addition::operator==(const Addition &other) const
{
    bool value = (hashString == other.hashString) && (prefixSize == other.prefixSize) && (compressionType == other.compressionType)
        && (riceDeltaEncoding == other.riceDeltaEncoding);
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
    : riceParameter(0)
    , numberEntries(0)
{
}

bool RiceDeltaEncoding::operator==(const RiceDeltaEncoding &other) const
{
    return (firstValue == other.firstValue) && (encodingData == other.encodingData) && (riceParameter == other.riceParameter)
        && (numberEntries == other.numberEntries);
}

bool RiceDeltaEncoding::isValid() const
{
    if (!firstValue.isEmpty() && !encodingData.isEmpty() && ((riceParameter >= 2 && riceParameter <= 28) || (riceParameter == 0 && numberEntries == 0))) {
        return true;
    }
    return false;
}
