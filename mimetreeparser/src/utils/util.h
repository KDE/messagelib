/*
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include <QString>

class QMimeType;

namespace KMime
{
class Content;
}

namespace MimeTreeParser
{
/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{
/**
 * Describes the type of the displayed message. This depends on the MIME structure
 * of the mail and on whether HTML mode is enabled (which is decided by htmlMail())
 */
enum HtmlMode {
    Normal, ///< A normal plaintext message, non-multipart
    Html, ///< A HTML message, non-multipart
    MultipartPlain, ///< A multipart/alternative message, the plain text part is currently displayed
    MultipartHtml, ///< A multipart/alternative message, the HTML part is currently displayed
    MultipartIcal ///< A multipart/alternative message, the ICal part is currently displayed
};

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT QString htmlModeToString(Util::HtmlMode mode);

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT bool isTypeBlacklisted(KMime::Content *node);

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT QString labelForContent(KMime::Content *node);

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT QMimeType mimetype(const QString &name);

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT QString iconNameForMimetype(const QString &mimeType,
                                                                    const QString &fallbackFileName1 = QString(),
                                                                    const QString &fallbackFileName2 = QString());

MIMETREEPARSER_EXPORT Q_REQUIRED_RESULT QString iconNameForContent(KMime::Content *node);
}
}

