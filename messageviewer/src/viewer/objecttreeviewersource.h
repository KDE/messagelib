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

#ifndef MAILVIEWER_OBJECTTREEVIEWERSOURCE_H
#define MAILVIEWER_OBJECTTREEVIEWERSOURCE_H

#include "objecttreeemptysource.h"

class QString;

namespace MessageViewer {
class AttachmentStrategy;
class HtmlWriter;
class ViewerPrivate;

/** An ObjectTreeParser source working on a MailViewer object */
class MailViewerSource : public MessageViewer::EmptySource
{
public:
    explicit MailViewerSource(ViewerPrivate *viewer);
    ~MailViewerSource();
    bool decryptMessage() const override;
    bool htmlLoadExternal() const override;
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;
    MimeTreeParser::Util::HtmlMode preferredMode() const override;
    int levelQuote() const override;
    const QTextCodec *overrideCodec() override;
    QString createMessageHeader(KMime::Message *message) override;
    const AttachmentStrategy *attachmentStrategy() const override;
    HtmlWriter *htmlWriter() const override;
    CSSHelperBase *cssHelper() const override;

    bool autoImportKeys() const override;
    bool showSignatureDetails() const override;
    bool showEmoticons() const override;
    bool showExpandQuotesMark() const override;
    bool isPrinting() const override;
private:
    ViewerPrivate *mViewer = nullptr;
};
}

#endif
