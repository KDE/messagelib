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

#ifndef UPDATEDATABASEINFO_H
#define UPDATEDATABASEINFO_H

#include "webengineviewer_export.h"
#include <QList>
#include <QVector>
#include <QObject>
namespace WebEngineViewer
{
struct Addition;
struct Removal;
struct WEBENGINEVIEWER_EXPORT UpdateDataBaseInfo {
    UpdateDataBaseInfo();
    bool isValid() const;

    enum ResponseType {
        Unknown = 0,
        FullUpdate = 1,
        PartialUpdate = 2
    };

    enum CompressionType {
        UnknownCompression = 0,
        RiceCompression = 1,
        RawCompression = 2
    };

    QVector<Addition> additionList;
    QVector<Removal> removalList;
    QString minimumWaitDuration;
    QString threatType;
    QString threatEntryType;
    ResponseType responseType;
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

#endif // UPDATEDATABASEINFO_H
