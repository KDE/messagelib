/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
#include <Libkleo/Enum>

namespace KMime
{
class Content;
}

namespace Akonadi
{
class MessageQueueJob;
}

namespace MessageComposer
{
namespace Util
{
/**
 * Sets the proper structural information such as content-type, cte, and charset on the encoded body content. Depending on the crypto options,
 *  original content may be needed to determine some of the values
 */
KMime::Content *
composeHeadersAndBody(KMime::Content *orig, QByteArray encodedBody, Kleo::CryptoMessageFormat format, bool sign, const QByteArray &hashAlgo = "pgp-sha1");

/**
 * Sets the content-type for the top level of the mime content, based on the crypto format and if a signature is used.
 */
void makeToplevelContentType(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign, const QByteArray &hashAlgo = "pgp-sha1");

/**
 * Sets the nested content type of the content, for crypto operations.
 */
void setNestedContentType(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign);

/**
 * Sets the nested content dispositions for the crypto operations.
 */
void setNestedContentDisposition(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign);

/**
 * Helper that returns whether or not the current combination of crypto format and signing choice means that the
 * resulting message will be a mime message or not.
 */
[[nodiscard]] bool makeMultiMime(Kleo::CryptoMessageFormat f, bool sign);
void addSendReplyForwardAction(const KMime::Message::Ptr &message, Akonadi::MessageQueueJob *qjob);
}
}
