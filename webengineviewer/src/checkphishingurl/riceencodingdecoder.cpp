/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

    RiceDecoder decoder(riceDeltaEncoding.riceParameter, riceDeltaEncoding.numberEntries, riceDeltaEncoding.encodingData);
    int lastValue(firstValue);
    bool result = false;
    while (decoder.hasOtherEntries()) {
        uint32_t offset;
        result = decoder.nextValue(&offset);
        if (!result) {
            return QList<int>();
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



QByteArray RiceEncodingDecoder::decodeRiceHashesDelta(const RiceDeltaEncoding &riceDeltaEncoding)
{
    QList<int> list;
    list.reserve(riceDeltaEncoding.numberEntries + 1);
    //TODO
    QByteArray ba;
    return ba;
}

RiceDecoder::RiceDecoder(int riceParameter, int numberEntries, const QByteArray& encodingData)
    : mEncodingData(encodingData),
      mRiceParameter(riceParameter),
      mNumberEntries(numberEntries)
{

}

RiceDecoder::~RiceDecoder()
{

}

bool RiceDecoder::hasOtherEntries() const
{
    return (mNumberEntries > 0);
}

bool RiceDecoder::nextValue(uint32_t* value)
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

bool RiceDecoder::nextBits(unsigned int num_requested_bits, uint32_t* x)
{
#if 0
    if (num_requested_bits > kMaxBitIndex) {
        return false;
    }

    if (current_word_bit_index_ == kMaxBitIndex) {
        bool result = nextWord(&current_word_);
        if (!result) {
            return false;
        }
    }

    unsigned int num_bits_left_in_current_word = kMaxBitIndex - current_word_bit_index_;
    if (num_bits_left_in_current_word >= num_requested_bits) {
        // All the bits that we need are in |current_word_|.
        *x = bitsFromCurrentWord(num_requested_bits);
    } else {
        // |current_word_| contains fewer bits than we need so read the remaining
        // bits from |current_word_| into |lower|, and then call nextBits on the
        // remaining number of bits, which will read in a new word into
        // |current_word_|.
        uint32_t lower = bitsFromCurrentWord(num_bits_left_in_current_word);

        unsigned int num_bits_from_next_word =
                num_requested_bits - num_bits_left_in_current_word;
        uint32_t upper;
        bool result = nextBits(num_bits_from_next_word, &upper);
        if (!result) {
            return false;
        }
        *x = (upper << num_bits_left_in_current_word) | lower;
    }
#endif
    return false;
}

bool RiceDecoder::nextWord(uint32_t* word)
{
#if 0
    if (data_byte_index_ >= data_.size()) {
        return false;
    }

    const size_t mask = 0xFF;
    *word = (data_[data_byte_index_] & mask);
    data_byte_index_++;
    current_word_bit_index_ = 0;

    if (data_byte_index_ < data_.size()) {
        *word |= ((data_[data_byte_index_] & mask) << 8);
        data_byte_index_++;

        if (data_byte_index_ < data_.size()) {
            *word |= ((data_[data_byte_index_] & mask) << 16);
            data_byte_index_++;

            if (data_byte_index_ < data_.size()) {
                *word |= ((data_[data_byte_index_] & mask) << 24);
                data_byte_index_++;
            }
        }
    }
#endif
    return true;
}

uint32_t RiceDecoder::bitsFromCurrentWord(unsigned int num_requested_bits)
{
#if 0
    uint32_t mask = 0xFFFFFFFF >> (kMaxBitIndex - num_requested_bits);
    uint32_t x = current_word_ & mask;
    current_word_ = current_word_ >> num_requested_bits;
    current_word_bit_index_ += num_requested_bits;
    return x;
#endif
    return {};
}

