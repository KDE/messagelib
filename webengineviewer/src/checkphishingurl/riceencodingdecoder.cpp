/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

   Code based in v4_rice.cc

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

#include <netinet/in.h>

namespace {
const int kBitsPerByte = 8;
const unsigned int kMaxBitIndex = kBitsPerByte * sizeof(uint32_t);
}

using namespace WebEngineViewer;
RiceEncodingDecoder::RiceEncodingDecoder()
{
}

RiceEncodingDecoder::~RiceEncodingDecoder()
{
}

QList<quint32> RiceEncodingDecoder::decodeRiceIndiceDelta(const RiceDeltaEncoding &riceDeltaEncoding)
{
    bool ok;
    QList<quint32> list;
    if (riceDeltaEncoding.firstValue.isEmpty()) {
        return list;
    }
    quint64 firstValue = riceDeltaEncoding.firstValue.toInt(&ok);
    if (!ok) {
        qCWarning(WEBENGINEVIEWER_LOG) << "First value is not a int value " << riceDeltaEncoding.firstValue;
        return QList<quint32>();
    }
    list.reserve(riceDeltaEncoding.numberEntries + 1);
    list << firstValue;
    if (riceDeltaEncoding.numberEntries == 0) {
        return list;
    }

    RiceDecoder decoder(riceDeltaEncoding.riceParameter, riceDeltaEncoding.numberEntries, riceDeltaEncoding.encodingData);
    int lastValue(firstValue);
    while (decoder.hasOtherEntries()) {
        quint32 offset;
        bool result = decoder.nextValue(&offset);
        if (!result) {
            return QList<quint32>();
        }
        lastValue += offset;
#if 0

        if (!last_value.IsValid()) {
            return false;
        }
#endif

        list << lastValue;
    }
    return list;
}

QList<quint32> RiceEncodingDecoder::decodeRiceHashesDelta(const RiceDeltaEncoding &riceDeltaEncoding)
{
    QList<quint32> list;
    bool ok = false;
    quint64 firstValue = riceDeltaEncoding.firstValue.toInt(&ok);
    if (!ok) {
        qCWarning(WEBENGINEVIEWER_LOG) << "First value is not a int value " << riceDeltaEncoding.firstValue;
        return list;
    }

    list.reserve(riceDeltaEncoding.numberEntries + 1);
    RiceDecoder decoder(riceDeltaEncoding.riceParameter, riceDeltaEncoding.numberEntries, riceDeltaEncoding.encodingData);
    int lastValue(firstValue);
    list << htonl(lastValue);

    while (decoder.hasOtherEntries()) {
        quint32 offset;
        bool result = decoder.nextValue(&offset);
        if (!result) {
            return QList<quint32>();
        }

        lastValue += offset;
#if 0
        if (!last_value) {
            return false;
        }
#endif
        // This flipping is done so that the decoded uint32 is interpreted
        // correcly as a string of 4 bytes.
        list << htonl(lastValue);
    }

    // Flipping the bytes, as done above, destroys the sort order. Sort the
    // values back.
    std::sort(list.begin(), list.end());

    // This flipping is done so that when the vector is interpreted as a string,
    // the bytes are in the correct order.
    QList<quint32> newList;
    newList.reserve(list.count());
    const int listCount(list.count());
    for (int i = 0; i < listCount; ++i) {
        newList << ntohl(list.at(i));
    }
    return newList;
}

RiceDecoder::RiceDecoder(int riceParameter, int numberEntries, const QByteArray &encodingData)
    : mEncodingData(encodingData)
    , mRiceParameter(riceParameter)
    , mNumberEntries(numberEntries)
    , mCurrentWord(0)
{
    mDataByteIndex = 0;
    mCurrentWordBitIndex = kMaxBitIndex;
}

RiceDecoder::~RiceDecoder()
{
}

bool RiceDecoder::hasOtherEntries() const
{
    return mNumberEntries > 0;
}

bool RiceDecoder::nextValue(uint32_t *value)
{
    if (!hasOtherEntries()) {
        return false;
    }
    bool result;
    uint32_t q = 0;
    uint32_t bit;
    do {
        result = nextBits(1, &bit);
        if (!result) {
            return false;
        }
        q += bit;
    } while (bit);
    uint32_t r = 0;
    result = nextBits(mRiceParameter, &r);
    if (!result) {
        return false;
    }

    *value = (q << mRiceParameter) + r;
    mNumberEntries--;
    return true;
}

bool RiceDecoder::nextBits(unsigned int numRequestedBits, uint32_t *x)
{
    if (numRequestedBits > kMaxBitIndex) {
        return false;
    }
    if (mCurrentWordBitIndex == kMaxBitIndex) {
        bool result = nextWord(&mCurrentWord);
        if (!result) {
            return false;
        }
    }
    unsigned int num_bits_left_in_current_word = kMaxBitIndex - mCurrentWordBitIndex;
    if (num_bits_left_in_current_word >= numRequestedBits) {
        // All the bits that we need are in |mCurrentWord|.
        *x = bitsFromCurrentWord(numRequestedBits);
    } else {
        // |mCurrentWord| contains fewer bits than we need so read the remaining
        // bits from |mCurrentWord| into |lower|, and then call nextBits on the
        // remaining number of bits, which will read in a new word into
        // |mCurrentWord|.
        uint32_t lower = bitsFromCurrentWord(num_bits_left_in_current_word);

        unsigned int num_bits_from_next_word
            = numRequestedBits - num_bits_left_in_current_word;
        uint32_t upper;
        bool result = nextBits(num_bits_from_next_word, &upper);
        if (!result) {
            return false;
        }
        *x = (upper << num_bits_left_in_current_word) | lower;
    }
    return true;
}

bool RiceDecoder::nextWord(uint32_t *word)
{
    if (mDataByteIndex >= mEncodingData.size()) {
        return false;
    }

    const size_t mask = 0xFF;
    *word = (mEncodingData[mDataByteIndex] & mask);
    mDataByteIndex++;
    mCurrentWordBitIndex = 0;

    if (mDataByteIndex < mEncodingData.size()) {
        *word |= ((mEncodingData[mDataByteIndex] & mask) << 8);
        mDataByteIndex++;

        if (mDataByteIndex < mEncodingData.size()) {
            *word |= ((mEncodingData[mDataByteIndex] & mask) << 16);
            mDataByteIndex++;

            if (mDataByteIndex < mEncodingData.size()) {
                *word |= ((mEncodingData[mDataByteIndex] & mask) << 24);
                mDataByteIndex++;
            }
        }
    }
    return true;
}

uint32_t RiceDecoder::bitsFromCurrentWord(unsigned int numRequestedBits)
{
    uint32_t mask = 0xFFFFFFFF >> (kMaxBitIndex - numRequestedBits);
    uint32_t x = mCurrentWord & mask;
    mCurrentWord = mCurrentWord >> numRequestedBits;
    mCurrentWordBitIndex += numRequestedBits;
    return x;
}
