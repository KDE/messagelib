/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "objecttreeemptysource.h"
class QString;

namespace MessageViewer
{
class AttachmentStrategy;
class HtmlWriter;
class ViewerPrivate;

/** An ObjectTreeParser source working on a MailViewer object */
class MailViewerSource : public MessageViewer::EmptySource
{
public:
    explicit MailViewerSource(ViewerPrivate *viewer);
    ~MailViewerSource() override;
    Q_REQUIRED_RESULT bool decryptMessage() const override;
    Q_REQUIRED_RESULT bool htmlLoadExternal() const override;
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;
    Q_REQUIRED_RESULT MimeTreeParser::Util::HtmlMode preferredMode() const override;
    Q_REQUIRED_RESULT int levelQuote() const override;
    const QTextCodec *overrideCodec() override;
    Q_REQUIRED_RESULT QString createMessageHeader(KMime::Message *message) override;
    const AttachmentStrategy *attachmentStrategy() const override;
    HtmlWriter *htmlWriter() const override;
    CSSHelperBase *cssHelper() const override;

    Q_REQUIRED_RESULT bool autoImportKeys() const override;
    Q_REQUIRED_RESULT bool showSignatureDetails() const override;
    Q_REQUIRED_RESULT bool showEncryptionDetails() const override;
    Q_REQUIRED_RESULT bool showEmoticons() const override;
    Q_REQUIRED_RESULT bool showExpandQuotesMark() const override;
    Q_REQUIRED_RESULT bool isPrinting() const override;

private:
    ViewerPrivate *const mViewer;
};
}

