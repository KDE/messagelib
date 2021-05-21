/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QList>
#include <QObject>
#include <QVector>
namespace WebEngineViewer
{
struct Addition;
struct Removal;
struct WEBENGINEVIEWER_EXPORT UpdateDataBaseInfo {
    UpdateDataBaseInfo();
    Q_REQUIRED_RESULT bool isValid() const;

    enum ResponseType { Unknown = 0, FullUpdate = 1, PartialUpdate = 2 };

    enum CompressionType { UnknownCompression = 0, RiceCompression = 1, RawCompression = 2 };

    QVector<Addition> additionList;
    QVector<Removal> removalList;
    QString minimumWaitDuration;
    QString threatType;
    QString threatEntryType;
    ResponseType responseType = Unknown;
    QString platformType;
    QString newClientState;
    QByteArray sha256;
    void clear();
    bool operator==(const UpdateDataBaseInfo &other) const;
};

struct WEBENGINEVIEWER_EXPORT RiceDeltaEncoding {
    RiceDeltaEncoding();
    bool operator==(const RiceDeltaEncoding &other) const;
    bool isValid() const;
    QByteArray firstValue;
    QByteArray encodingData;
    int riceParameter;
    int numberEntries;
};

struct WEBENGINEVIEWER_EXPORT Addition {
    Addition();
    bool isValid() const;
    bool operator==(const Addition &other) const;

    static bool lessThan(const Addition &s1, const Addition &s2);
    QByteArray hashString;
    RiceDeltaEncoding riceDeltaEncoding;
    UpdateDataBaseInfo::CompressionType compressionType;
    int prefixSize;
};

struct WEBENGINEVIEWER_EXPORT Removal {
    Removal();
    bool operator==(const Removal &other) const;
    bool isValid() const;
    QList<quint32> indexes;
    RiceDeltaEncoding riceDeltaEncoding;
    UpdateDataBaseInfo::CompressionType compressionType;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo)
Q_DECLARE_METATYPE(WebEngineViewer::UpdateDataBaseInfo::CompressionType)
Q_DECLARE_TYPEINFO(WebEngineViewer::Addition, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::Removal, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(WebEngineViewer::RiceDeltaEncoding, Q_MOVABLE_TYPE);

