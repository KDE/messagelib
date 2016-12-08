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
    RiceDecoder(int riceParameter, int numberEntries, const QByteArray &encodingData);
    ~RiceDecoder();

    bool hasOtherEntries() const;
    bool nextValue(uint32_t *value);
    bool nextBits(unsigned int num_requested_bits, uint32_t *x);
    uint32_t bitsFromCurrentWord(unsigned int num_requested_bits);
    bool nextWord(uint32_t *word);
private:
    QByteArray mEncodingData;
    int mRiceParameter;
    int mNumberEntries;

    // Represents how many total bytes have we read from |data_| into
    // |mCurrentWord|.
    int mDataByteIndex;

    // Represents the number of bits that we have read from |mCurrentWord|. When
    // this becomes 32, which is the size of |mCurrentWord|, a new
    // |mCurrentWord| needs to be read from |data_|.
    unsigned int mCurrentWordBitIndex;

    // The 32-bit value read from |data_|. All bit reading operations operate on
    // |mCurrentWord|.
    uint32_t mCurrentWord;

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
