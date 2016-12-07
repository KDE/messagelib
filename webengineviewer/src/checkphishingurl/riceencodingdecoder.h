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

#ifndef RICEENCODINGDECODER_H
#define RICEENCODINGDECODER_H

#include "webengineviewer_export.h"
#include "updatedatabaseinfo.h"
namespace WebEngineViewer
{
//https://developers.google.com/safe-browsing/v4/compression
class RiceDecoder
{
public:
    RiceDecoder(int riceParameter, int numberEntries, const QByteArray& encodingData);
    ~RiceDecoder();

    bool hasOtherEntries() const;
    void nextValue(uint32_t *value);
    void nextBits(unsigned int num_requested_bits, uint32_t *x);
private:
    QByteArray mEncodingData;
    int mRiceParameter;
    int mNumberEntries;
};

class WEBENGINEVIEWER_EXPORT RiceEncodingDecoder
{
public:
    RiceEncodingDecoder();
    ~RiceEncodingDecoder();

    static QList<int> decodeRiceIndiceDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
    static QByteArray decodeRiceHashesDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
};
}

#endif // RICEENCODINGDECODER_H
