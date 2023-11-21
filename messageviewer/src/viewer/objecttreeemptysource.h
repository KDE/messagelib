/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <MimeTreeParser/ObjectTreeSource>

class QString;
class QStringDecoder;
namespace MessageViewer
{
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
    [[nodiscard]] bool decryptMessage() const override;
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;
    [[nodiscard]] MimeTreeParser::Util::HtmlMode preferredMode() const override;
    void setAllowDecryption(bool allowDecryption);
    QByteArray overrideCodecName() const override;
    virtual QString createMessageHeader(KMime::Message *message);
    [[nodiscard]] bool autoImportKeys() const override;

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
    std::unique_ptr<EmptySourcePrivate> const d;
};
}
