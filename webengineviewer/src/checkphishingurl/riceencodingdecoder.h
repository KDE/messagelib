/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "updatedatabaseinfo.h"
#include "webengineviewer_private_export.h"
#include <QList>
namespace WebEngineViewer
{
// https://developers.google.com/safe-browsing/v4/compression
class RiceDecoder
{
public:
    RiceDecoder(int riceParameter, int numberEntries, const QByteArray &encodingData);
    ~RiceDecoder();

    [[nodiscard]] bool hasOtherEntries() const;
    [[nodiscard]] bool nextValue(uint32_t *value);
    [[nodiscard]] bool nextBits(unsigned int num_requested_bits, uint32_t *x);
    [[nodiscard]] uint32_t bitsFromCurrentWord(unsigned int num_requested_bits);
    [[nodiscard]] bool nextWord(uint32_t *word);

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

class WEBENGINEVIEWER_TESTS_EXPORT RiceEncodingDecoder
{
public:
    RiceEncodingDecoder();
    ~RiceEncodingDecoder();

    [[nodiscard]] static QList<quint32> decodeRiceIndiceDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
    [[nodiscard]] static QList<quint32> decodeRiceHashesDelta(const WebEngineViewer::RiceDeltaEncoding &riceDeltaEncoding);
};
}
