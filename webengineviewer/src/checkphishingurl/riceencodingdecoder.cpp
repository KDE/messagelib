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

#include "riceencodingdecoder.h"
#include "webengineviewer_debug.h"

using namespace WebEngineViewer;
RiceEncodingDecoder::RiceEncodingDecoder()
{

}

RiceEncodingDecoder::~RiceEncodingDecoder()
{

}

QList<int> RiceEncodingDecoder::decodeRiceIndiceDelta(const RiceDeltaEncoding &riceDeltaEncoding)
{
    bool ok;
    QList<int> list;
    if (riceDeltaEncoding.firstValue.isEmpty()) {
        return list;
    }
    quint64 firstValue = riceDeltaEncoding.firstValue.toInt(&ok);
    if (!ok) {
        qCWarning(WEBENGINEVIEWER_LOG) << "First value is not a int value " << riceDeltaEncoding.firstValue;
        return list;
    }
    list.reserve(riceDeltaEncoding.numberEntries + 1);
    list << firstValue;
    if (riceDeltaEncoding.numberEntries == 0) {
        return list;
    }

    return {};
    //TODO
}

QByteArray RiceEncodingDecoder::decodeRiceHashesDelta(const RiceDeltaEncoding &riceDeltaEncoding)
{
    QList<int> list;
    list.reserve(riceDeltaEncoding.numberEntries + 1);
    return {};
    //TODO
}
