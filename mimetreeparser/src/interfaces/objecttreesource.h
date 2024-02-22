/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"
#include <MimeTreeParser/Util>

#include <KMime/Message>
#include <QSharedPointer>
#include <QStringDecoder>

namespace MimeTreeParser
{
class BodyPartFormatterFactory;
class MessagePart;
using MessagePartPtr = QSharedPointer<MessagePart>;
}

namespace MimeTreeParser
{
namespace Interface
{
/**
 * Interface for object tree sources.
 * @author Andras Mantia <amantia@kdab.net>
 */
class MIMETREEPARSER_EXPORT ObjectTreeSource
{
public:
    ObjectTreeSource();
    virtual ~ObjectTreeSource();

    /**
     * Sets the type of mail that is currently displayed. Applications can display this
     * information to the user, for example KMail displays a HTML status bar.
     * Note: This is not called when the mode is "Normal".
     */
    virtual void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) = 0;

    /** Return the mode that is the preferred to display */
    virtual MimeTreeParser::Util::HtmlMode preferredMode() const = 0;

    /** Return true if an encrypted mail should be decrypted */
    [[nodiscard]] virtual bool decryptMessage() const = 0;

    /** The override codec that should be used for the mail */
    [[nodiscard]] virtual QByteArray overrideCodecName() const = 0;

    /** should keys be imported automatically **/
    [[nodiscard]] virtual bool autoImportKeys() const = 0;

    virtual const BodyPartFormatterFactory *bodyPartFormatterFactory() = 0;

private:
    Q_DISABLE_COPY(ObjectTreeSource)
};
}
}
