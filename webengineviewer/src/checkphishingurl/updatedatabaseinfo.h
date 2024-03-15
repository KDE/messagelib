/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
    UpdateDataBaseInfo();
    [[nodiscard]] bool isValid() const;

    enum ResponseType {
        Unknown = 0,
        FullUpdate = 1,
        PartialUpdate = 2,
    };

    enum CompressionType {
        UnknownCompression = 0,
        RiceCompression = 1,
        RawCompression = 2,
    };

    QList<Addition> additionList;
    QList<Removal> removalList;
    QString minimumWaitDuration;
    QString threatType;
    QString threatEntryType;
    ResponseType responseType = Unknown;
    QString platformType;
    QString newClientState;
    QByteArray sha256;
    void clear();
    [[nodiscard]] bool operator==(const UpdateDataBaseInfo &other) const;
};

struct WEBENGINEVIEWER_EXPORT RiceDeltaEncoding {
    RiceDeltaEncoding();
    [[nodiscard]] bool operator==(const RiceDeltaEncoding &other) const;
    [[nodiscard]] bool isValid() const;
    QByteArray firstValue;
    QByteArray encodingData;
    int riceParameter;
    int numberEntries;
};

struct WEBENGINEVIEWER_EXPORT Addition {
    Addition();
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] bool operator==(const Addition &other) const;

    [[nodiscard]] static bool lessThan(const Addition &s1, const Addition &s2);
    QByteArray hashString;
    RiceDeltaEncoding riceDeltaEncoding;
    UpdateDataBaseInfo::CompressionType compressionType;
    int prefixSize;
};

struct WEBENGINEVIEWER_EXPORT Removal {
    Removal();
    [[nodiscard]] bool operator==(const Removal &other) const;
    [[nodiscard]] bool isValid() const;
    QList<quint32> indexes;
    RiceDeltaEncoding riceDeltaEncoding;
    UpdateDataBaseInfo::CompressionType compressionType;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo)
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo::CompressionType)
Q_DECLARE_TYPEINFO(WebEngineViewer::Addition, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::Removal, Q_RELOCATABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::RiceDeltaEncoding, Q_RELOCATABLE_TYPE);
