/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "objecttreeemptysource.h"
#include <QStringDecoder>
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
    [[nodiscard]] bool decryptMessage() const override;
    [[nodiscard]] bool htmlLoadExternal() const override;
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;
    [[nodiscard]] MimeTreeParser::Util::HtmlMode preferredMode() const override;
    [[nodiscard]] int levelQuote() const override;
    QByteArray overrideCodecName() const override;

    [[nodiscard]] QString createMessageHeader(KMime::Message *message) override;
    const AttachmentStrategy *attachmentStrategy() const override;
    HtmlWriter *htmlWriter() const override;
    CSSHelperBase *cssHelper() const override;

    [[nodiscard]] bool autoImportKeys() const override;
    [[nodiscard]] bool showSignatureDetails() const override;
    [[nodiscard]] bool showEncryptionDetails() const override;
    [[nodiscard]] bool showEmoticons() const override;
    [[nodiscard]] bool showExpandQuotesMark() const override;
    [[nodiscard]] bool isPrinting() const override;

private:
    ViewerPrivate *const mViewer;
};
}
