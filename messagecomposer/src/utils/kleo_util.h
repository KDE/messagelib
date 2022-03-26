/*  -*- c++ -*-
    kleo_util.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Libkleo/Enum>

static const Kleo::CryptoMessageFormat cryptoMessageFormats[] = {Kleo::AutoFormat,
                                                                 Kleo::InlineOpenPGPFormat,
                                                                 Kleo::OpenPGPMIMEFormat,
                                                                 Kleo::SMIMEFormat,
                                                                 Kleo::SMIMEOpaqueFormat};
static const int numCryptoMessageFormats = sizeof cryptoMessageFormats / sizeof *cryptoMessageFormats;

static const Kleo::CryptoMessageFormat concreteCryptoMessageFormats[] = {Kleo::OpenPGPMIMEFormat,
                                                                         Kleo::SMIMEFormat,
                                                                         Kleo::SMIMEOpaqueFormat,
                                                                         Kleo::InlineOpenPGPFormat};
static const unsigned int numConcreteCryptoMessageFormats = sizeof concreteCryptoMessageFormats / sizeof *concreteCryptoMessageFormats;

static inline Kleo::CryptoMessageFormat cb2format(int idx)
{
    return cryptoMessageFormats[idx >= 0 && idx < numCryptoMessageFormats ? idx : 0];
}

static inline int format2cb(Kleo::CryptoMessageFormat f)
{
    for (int i = 0; i < numCryptoMessageFormats; ++i) {
        if (f == cryptoMessageFormats[i]) {
            return i;
        }
    }
    return 0;
}

//
// some helper functions indicating the need for CryptoMessageFormat
// to be a class type :)
//

static inline bool isSMIME(Kleo::CryptoMessageFormat f)
{
    return f == Kleo::SMIMEFormat || f == Kleo::SMIMEOpaqueFormat;
}

static inline bool isOpenPGP(Kleo::CryptoMessageFormat f)
{
    return f == Kleo::InlineOpenPGPFormat || f == Kleo::OpenPGPMIMEFormat;
}

static inline bool containsSMIME(unsigned int f)
{
    return f & (Kleo::SMIMEFormat | Kleo::SMIMEOpaqueFormat);
}

static inline bool containsOpenPGP(unsigned int f)
{
    return f & (Kleo::OpenPGPMIMEFormat | Kleo::InlineOpenPGPFormat);
}
