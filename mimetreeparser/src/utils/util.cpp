/*
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "util.h"
using namespace Qt::Literals::StringLiterals;

#include "mimetreeparser_debug.h"

#include "nodehelper.h"

#include <KMime/Content>

#include <QMimeDatabase>

using namespace MimeTreeParser;
using namespace MimeTreeParser::Util;

bool MimeTreeParser::Util::isTypeBlacklisted(const KMime::Content *node)
{
    const auto contentType = node->contentType();
    const QByteArray mediaTypeLower = contentType ? contentType->mediaType().toLower() : QByteArray();
    bool typeBlacklisted = mediaTypeLower == QByteArrayLiteral("multipart");
    if (!typeBlacklisted) {
        typeBlacklisted = KMime::isCryptoPart(node);
    }
    typeBlacklisted = typeBlacklisted || node == node->topLevel();
    const bool firstTextChildOfEncapsulatedMsg = mediaTypeLower == "text" && contentType && contentType->subType().toLower() == "plain" && node->parent()
        && node->parent()->contentType() && node->parent()->contentType()->mediaType().toLower() == "message";
    return typeBlacklisted || firstTextChildOfEncapsulatedMsg;
}

QString MimeTreeParser::Util::labelForContent(const KMime::Content *node)
{
    const auto ct = node->contentType();
    const QString name = ct ? ct->name() : QString();
    QString label = name.isEmpty() ? NodeHelper::fileName(node) : name;
    if (label.isEmpty()) {
        const auto cd = node->contentDescription();
        label = cd ? cd->asUnicodeString() : QString();
    }
    return label;
}

QMimeType MimeTreeParser::Util::mimetype(const QString &name)
{
    QMimeDatabase db;
    // consider the filename if mimetype cannot be found by content-type
    const auto mimeTypes = db.mimeTypesForFileName(name);
    for (const auto &mt : mimeTypes) {
        if (mt.name() != QLatin1StringView("application/octet-stream")) {
            return mt;
        }
    }

    // consider the attachment's contents if neither the Content-Type header
    // nor the filename give us a clue
    return db.mimeTypeForFile(name);
}

QString MimeTreeParser::Util::iconNameForMimetype(const QString &mimeType, const QString &fallbackFileName1, const QString &fallbackFileName2)
{
    QString fileName;
    QString tMimeType = mimeType;

    // convert non-registered types to registered types
    if (mimeType == QLatin1StringView("application/x-vnd.kolab.contact")) {
        tMimeType = u"text/x-vcard"_s;
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.event")) {
        tMimeType = u"application/x-vnd.akonadi.calendar.event"_s;
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.task")) {
        tMimeType = u"application/x-vnd.akonadi.calendar.todo"_s;
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.journal")) {
        tMimeType = u"application/x-vnd.akonadi.calendar.journal"_s;
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.note")) {
        tMimeType = u"application/x-vnd.akonadi.note"_s;
    } else if (mimeType == QLatin1StringView("image/jpg")) {
        tMimeType = u"image/jpeg"_s;
    } else if (mimeType == QLatin1StringView("application/x-pkcs7-signature")) {
        tMimeType = u"application/pkcs7-signature"_s;
    } else if (mimeType == QLatin1StringView("message/global")) {
        tMimeType = u"message/rfc822"_s;
    } else if (mimeType == QLatin1StringView("text/x-moz-deleted")) {
        // Avoid debug warning about unknown mimetype
        // Bug: 468801
        // We need to show unknown icon
        tMimeType.clear();
        fileName = u"unknown"_s;
    }
    QMimeDatabase mimeDb;
    if (!tMimeType.isEmpty()) {
        auto mime = mimeDb.mimeTypeForName(tMimeType);
        if (mime.isValid()) {
            fileName = mime.iconName();
        } else {
            fileName = u"unknown"_s;
            if (!tMimeType.isEmpty()) {
                qCWarning(MIMETREEPARSER_LOG) << "unknown mimetype" << tMimeType;
            }
        }
    }
    // WorkAround for #199083
    if (fileName == QLatin1StringView("text-vcard")) {
        fileName = u"text-x-vcard"_s;
    }

    if (fileName.isEmpty()) {
        fileName = fallbackFileName1;
        if (fileName.isEmpty()) {
            fileName = fallbackFileName2;
        }
        if (!fileName.isEmpty()) {
            fileName = mimeDb.mimeTypeForFile(QLatin1StringView("/tmp/") + fileName).iconName();
        }
    }

    return fileName;
}

QString MimeTreeParser::Util::iconNameForContent(const KMime::Content *node)
{
    if (!node) {
        return {};
    }

    const auto ct = node->contentType();
    QByteArray mimeType = ct ? ct->mimeType() : QByteArray();
    const auto cd = node->contentDisposition();
    if (mimeType.isNull() || mimeType == "application/octet-stream") {
        const QString fileName = cd ? cd->filename() : QString();
        if (!fileName.isEmpty()) {
            const QString mime = MimeTreeParser::Util::mimetype(fileName).name();
            mimeType = mime.toLatin1();
        }
    }
    mimeType = mimeType.toLower();
    return MimeTreeParser::Util::iconNameForMimetype(QLatin1StringView(mimeType), cd ? cd->filename() : QString(), ct ? ct->name() : QString());
}

QString MimeTreeParser::Util::htmlModeToString(HtmlMode mode)
{
    switch (mode) {
    case Normal: ///< A normal plaintext message, non-multipart
        return u"Normal PlainText Message, non-multipart"_s;
    case Html: ///< A HTML message, non-multipart
        return u"A HTML message, non-multipart"_s;
    case MultipartPlain: ///< A multipart/alternative message, the plain text part is currently displayed
        return u"A multipart/alternative message, the plain text part is currently displayed"_s;
    case MultipartHtml: ///< A multipart/alternative message, the HTML part is currently displayed
        return u"A multipart/alternative message, the HTML part is currently displayed"_s;
    case MultipartIcal: ///< A multipart/alternative message, the ICal part is currently displayed
        return u"A multipart/alternative message, the ICal part is currently displayed"_s;
    }
    return {};
}
