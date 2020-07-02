/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef RICEENCODINGDECODERTEST_H
#define RICEENCODINGDECODERTEST_H

#include <QObject>

class RiceEncodingDecoderTest : public QObject
{
    Q_OBJECT
public:
    explicit RiceEncodingDecoderTest(QObject *parent = nullptr);
    ~RiceEncodingDecoderTest();

private Q_SLOTS:
    void shouldDecodeRiceIndices_data();
    void shouldDecodeRiceIndices();

    void shouldDecodeRiceHashes_data();
    void shouldDecodeRiceHashes();
};

#endif // RICEENCODINGDECODERTEST_H
