/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Headers>
#include <Libkleo/Enum>
#include <QByteArray>

namespace KMime
{
class Content;
}

namespace ComposerTestUtil
{
/**
 * gate function to run verifySignature, verifyEncryption or verifySignatureAndEncryption.
 */

void verify(bool sign,
            bool encrypt,
            KMime::Content *content,
            const QByteArray &origContent,
            Kleo::CryptoMessageFormat f,
            KMime::Headers::contentEncoding encoding);

/**
 * Verifies that the given MIME content is signed and that the text is equal
 */
void verifySignature(KMime::Content *content, const QByteArray &signedContent, Kleo::CryptoMessageFormat f, KMime::Headers::contentEncoding encoding);

/**
 * Verifies that the given MIME content is encrypted, and that the text is equal
 */
void verifyEncryption(KMime::Content *content, const QByteArray &encrContent, Kleo::CryptoMessageFormat f, bool withAttachment = false);

/**
 * Verifies that the given MIME content is signed and then encrypted, and the original text is as specified
 */
void verifySignatureAndEncryption(KMime::Content *content,
                                  const QByteArray &origContent,
                                  Kleo::CryptoMessageFormat f,
                                  bool withAttachment = false,
                                  bool combined = false);
}
