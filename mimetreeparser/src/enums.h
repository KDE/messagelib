/*
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

namespace MimeTreeParser
{
/**
 * The display update mode: Force updates the display immediately, Delayed updates
 * after some time (150ms by default)
 */
enum UpdateMode {
    Force = 0,
    Delayed,
};

/** Flags for the encryption state. */
typedef enum {
    KMMsgEncryptionStateUnknown = ' ',
    KMMsgNotEncrypted = 'N',
    KMMsgPartiallyEncrypted = 'P',
    KMMsgFullyEncrypted = 'F',
    KMMsgEncryptionProblematic = 'X'
} KMMsgEncryptionState;

/** Flags for the signature state. */
typedef enum {
    KMMsgSignatureStateUnknown = ' ',
    KMMsgNotSigned = 'N',
    KMMsgPartiallySigned = 'P',
    KMMsgFullySigned = 'F',
    KMMsgSignatureProblematic = 'X'
} KMMsgSignatureState;
}
