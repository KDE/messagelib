/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class RiceEncodingDecoderTest : public QObject
{
    Q_OBJECT
public:
    explicit RiceEncodingDecoderTest(QObject *parent = nullptr);
    ~RiceEncodingDecoderTest() override;

private Q_SLOTS:
    void shouldDecodeRiceIndices_data();
    void shouldDecodeRiceIndices();

    void shouldDecodeRiceHashes_data();
    void shouldDecodeRiceHashes();
};
