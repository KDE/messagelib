/*
  Copyright (C) 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Andras Mantia <andras@kdab.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef MAILVIEWER_OBJECTTREEEMPTYSOURCE_H
#define MAILVIEWER_OBJECTTREEEMPTYSOURCE_H

#include <MimeTreeParser/ObjectTreeSource>
#include "messageviewer_export.h"
class QString;

namespace MessageViewer {
class AttachmentStrategy;
class CSSHelperBase;
class HtmlWriter;

/** An ObjectTreeSource that does not work on anything */
class EmptySourcePrivate;
/**
 * @brief The EmptySource class
 */
class MESSAGEVIEWER_EXPORT EmptySource : public MimeTreeParser::Interface::ObjectTreeSource
{
public:
    EmptySource();
    ~EmptySource() override;
    Q_REQUIRED_RESULT bool decryptMessage() const override;
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;
    Q_REQUIRED_RESULT MimeTreeParser::Util::HtmlMode preferredMode() const override;
    void setAllowDecryption(bool allowDecryption);
    const QTextCodec *overrideCodec() override;
    virtual QString createMessageHeader(KMime::Message *message);
    Q_REQUIRED_RESULT bool autoImportKeys() const override;

    const MimeTreeParser::BodyPartFormatterFactory *bodyPartFormatterFactory() override;
    void render(const MimeTreeParser::MessagePartPtr &msgPart, bool showOnlyOneMimePart);

    virtual const AttachmentStrategy *attachmentStrategy() const;
    virtual HtmlWriter *htmlWriter() const;
    virtual CSSHelperBase *cssHelper() const;
    /** Return true if external sources should be loaded in a html mail */
    virtual bool htmlLoadExternal() const;
    virtual bool showSignatureDetails() const;
    virtual bool showEncryptionDetails() const;
    virtual bool showEmoticons() const;
    virtual bool showExpandQuotesMark() const;
    virtual bool isPrinting() const;
    virtual int levelQuote() const;
private:
    EmptySourcePrivate *const d;
};
}

#endif
