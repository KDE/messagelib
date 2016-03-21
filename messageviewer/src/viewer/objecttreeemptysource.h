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

#include <MimeTreeParser/ObjectTreeSourceIf>
#include "messageviewer_export.h"
class QString;

namespace MessageViewer
{

/** An ObjectTreeSource that does not work on anything */
class EmptySourcePrivate;
class MESSAGEVIEWER_EXPORT EmptySource : public ObjectTreeSourceIf
{
public:
    EmptySource();
    ~EmptySource();
    bool htmlMail() const Q_DECL_OVERRIDE;
    bool decryptMessage() const Q_DECL_OVERRIDE;
    bool htmlLoadExternal() const Q_DECL_OVERRIDE;
    bool showSignatureDetails() const Q_DECL_OVERRIDE;
    void setHtmlMode(MessageViewer::Util::HtmlMode mode) Q_DECL_OVERRIDE;
    void setAllowDecryption(bool allowDecryption);
    int levelQuote() const Q_DECL_OVERRIDE;
    const QTextCodec *overrideCodec() Q_DECL_OVERRIDE;
    QString createMessageHeader(KMime::Message *message) Q_DECL_OVERRIDE;
    const AttachmentStrategy *attachmentStrategy() Q_DECL_OVERRIDE;
    HtmlWriter *htmlWriter() Q_DECL_OVERRIDE;
    CSSHelperBase *cssHelper() Q_DECL_OVERRIDE;
    QObject *sourceObject() Q_DECL_OVERRIDE;

    bool autoImportKeys() const Q_DECL_OVERRIDE;
    bool showEmoticons() const Q_DECL_OVERRIDE;
    bool showExpandQuotesMark() const Q_DECL_OVERRIDE;
    const BodyPartFormatterBaseFactory *bodyPartFormatterFactory() Q_DECL_OVERRIDE;
private:
    EmptySourcePrivate *const d;
};

}

#endif

