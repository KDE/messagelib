/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QList>
#include <QObject>
namespace WebEngineViewer
{
struct Addition;
struct Removal;
struct WEBENGINEVIEWER_EXPORT UpdateDataBaseInfo {
    /*! Constructs an UpdateDataBaseInfo object. */
    UpdateDataBaseInfo();
    /*! Returns whether this is a valid update info. */
    [[nodiscard]] bool isValid() const;

    enum ResponseType : uint8_t {
        Unknown = 0,
        FullUpdate = 1,
        PartialUpdate = 2,
    };

    enum CompressionType : uint8_t {
        UnknownCompression = 0,
        RiceCompression = 1,
        RawCompression = 2,
    };

    /*! List of additions to apply. */
    QList<Addition> additionList;
    /*! List of removals to apply. */
    QList<Removal> removalList;
    /*! Minimum wait duration before next update. */
    QString minimumWaitDuration;
    /*! Type of threat. */
    QString threatType;
    /*! Type of threat entry. */
    QString threatEntryType;
    /*! Type of response. */
    ResponseType responseType = Unknown;
    /*! Platform type. */
    QString platformType;
    /*! New client state value. */
    QString newClientState;
    /*! SHA256 hash. */
    QByteArray sha256;
    /*! Clears the data. */
    void clear();
    /*! Compares this object with another. */
    [[nodiscard]] bool operator==(const UpdateDataBaseInfo &other) const;
};

struct WEBENGINEVIEWER_EXPORT RiceDeltaEncoding {
    /*! Constructs a RiceDeltaEncoding object. */
    RiceDeltaEncoding();
    /*! Compares this object with another. */
    [[nodiscard]] bool operator==(const RiceDeltaEncoding &other) const;
    /*! Returns whether this is a valid encoding. */
    [[nodiscard]] bool isValid() const;
    /*! First value in the encoding. */
    QByteArray firstValue;
    /*! Encoded data. */
    QByteArray encodingData;
    /*! Rice parameter for encoding. */
    int riceParameter;
    /*! Number of entries in the encoding. */
    int numberEntries;
};

struct WEBENGINEVIEWER_EXPORT Addition {
    /*! Constructs an Addition object. */
    Addition();
    /*! Returns whether this is a valid addition. */
    [[nodiscard]] bool isValid() const;
    /*! Compares this object with another. */
    [[nodiscard]] bool operator==(const Addition &other) const;

    /*! Compares two Addition objects for sorting. */
    [[nodiscard]] static bool lessThan(const Addition &s1, const Addition &s2);
    /*! Hash string for this addition. */
    QByteArray hashString;
    /*! Rice delta encoding for this addition. */
    RiceDeltaEncoding riceDeltaEncoding;
    /*! Compression type used. */
    UpdateDataBaseInfo::CompressionType compressionType;
    /*! Prefix size for the hash. */
    int prefixSize;
};

struct WEBENGINEVIEWER_EXPORT Removal {
    /*! Constructs a Removal object. */
    Removal();
    /*! Compares this object with another. */
    [[nodiscard]] bool operator==(const Removal &other) const;
    /*! Returns whether this is a valid removal. */
    [[nodiscard]] bool isValid() const;
    /*! List of indexes to remove. */
    QList<quint32> indexes;
    /*! Rice delta encoding for this removal. */
    RiceDeltaEncoding riceDeltaEncoding;
    /*! Compression type used. */
    UpdateDataBaseInfo::CompressionType compressionType;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo)
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo::CompressionType)
Q_DECLARE_TYPEINFO(WebEngineViewer::Addition, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::Removal, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::RiceDeltaEncoding, Q_RELOCATABLE_TYPE);
