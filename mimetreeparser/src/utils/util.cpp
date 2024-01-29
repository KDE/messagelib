/*
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "util.h"

#include "mimetreeparser_debug.h"

#include "nodehelper.h"

#include <KMime/Content>

#include <QMimeDatabase>

using namespace MimeTreeParser;
using namespace MimeTreeParser::Util;

bool MimeTreeParser::Util::isTypeBlacklisted(KMime::Content *node)
{
    const auto contentType = node->contentType(); // Create
    const QByteArray mediaTypeLower = contentType->mediaType().toLower();
    bool typeBlacklisted = mediaTypeLower == QByteArrayLiteral("multipart");
    if (!typeBlacklisted) {
        typeBlacklisted = KMime::isCryptoPart(node);
    }
    typeBlacklisted = typeBlacklisted || node == node->topLevel();
    const bool firstTextChildOfEncapsulatedMsg = mediaTypeLower == "text" && contentType->subType().toLower() == "plain" && node->parent()
        && node->parent()->contentType()->mediaType().toLower() == "message";
    return typeBlacklisted || firstTextChildOfEncapsulatedMsg;
}

QString MimeTreeParser::Util::labelForContent(KMime::Content *node)
{
    const QString name = node->contentType()->name();
    QString label = name.isEmpty() ? NodeHelper::fileName(node) : name;
    if (label.isEmpty()) {
        label = node->contentDescription()->asUnicodeString();
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
        tMimeType = QStringLiteral("text/x-vcard");
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.event")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.event");
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.task")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.todo");
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.journal")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.calendar.journal");
    } else if (mimeType == QLatin1StringView("application/x-vnd.kolab.note")) {
        tMimeType = QStringLiteral("application/x-vnd.akonadi.note");
    } else if (mimeType == QLatin1StringView("image/jpg")) {
        tMimeType = QStringLiteral("image/jpeg");
    } else if (mimeType == QLatin1StringView("application/x-pkcs7-signature")) {
        tMimeType = QStringLiteral("application/pkcs7-signature");
    } else if (mimeType == QLatin1StringView("message/global")) {
        tMimeType = QStringLiteral("message/rfc822");
    } else if (mimeType == QLatin1StringView("text/x-moz-deleted")) {
        // Avoid debug warning about unknown mimetype
        // Bug: 468801
        // We need to show unknown icon
        tMimeType.clear();
        fileName = QStringLiteral("unknown");
    }
    QMimeDatabase mimeDb;
    if (!tMimeType.isEmpty()) {
        auto mime = mimeDb.mimeTypeForName(tMimeType);
        if (mime.isValid()) {
            fileName = mime.iconName();
        } else {
            fileName = QStringLiteral("unknown");
            if (!tMimeType.isEmpty()) {
                qCWarning(MIMETREEPARSER_LOG) << "unknown mimetype" << tMimeType;
            }
        }
    }
    // WorkAround for #199083
    if (fileName == QLatin1StringView("text-vcard")) {
        fileName = QStringLiteral("text-x-vcard");
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

QString MimeTreeParser::Util::iconNameForContent(KMime::Content *node)
{
    if (!node) {
        return {};
    }

    auto ct = node->contentType(); // Create
    QByteArray mimeType = ct->mimeType();
    if (mimeType.isNull() || mimeType == "application/octet-stream") {
        const QString fileName = node->contentDisposition()->filename();
        if (!fileName.isEmpty()) {
            const QString mime = MimeTreeParser::Util::mimetype(fileName).name();
            mimeType = mime.toLatin1();
        }
    }
    mimeType = mimeType.toLower();
    return MimeTreeParser::Util::iconNameForMimetype(QLatin1StringView(mimeType), node->contentDisposition()->filename(), ct->name());
}

QString MimeTreeParser::Util::htmlModeToString(HtmlMode mode)
{
    switch (mode) {
    case Normal: ///< A normal plaintext message, non-multipart
        return QStringLiteral("Normal PlainText Message, non-multipart");
    case Html: ///< A HTML message, non-multipart
        return QStringLiteral("A HTML message, non-multipart");
    case MultipartPlain: ///< A multipart/alternative message, the plain text part is currently displayed
        return QStringLiteral("A multipart/alternative message, the plain text part is currently displayed");
    case MultipartHtml: ///< A multipart/alternative message, the HTML part is currently displayed
        return QStringLiteral("A multipart/alternative message, the HTML part is currently displayed");
    case MultipartIcal: ///< A multipart/alternative message, the ICal part is currently displayed
        return QStringLiteral("A multipart/alternative message, the ICal part is currently displayed");
    }
    return {};
}
