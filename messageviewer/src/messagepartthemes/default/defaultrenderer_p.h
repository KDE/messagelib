/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messagepartrendererbase.h"

#include <MimeTreeParser/MessagePart>

namespace MessageViewer
{
class AttachmentStrategy;
class MessagePartRendererFactory;
class CSSHelperBase;
class HtmlWriter;

class DefaultRendererPrivate : public RenderContext
{
    using RenderContext::renderWithFactory;

public:
    DefaultRendererPrivate(CSSHelperBase *cssHelper, const MessagePartRendererFactory *rendererFactory);
    ~DefaultRendererPrivate() override;

    CSSHelperBase *cssHelper() const override;

    MimeTreeParser::Interface::ObjectTreeSource *source() const;

    void renderSubParts(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) override;

    void render(const MimeTreeParser::MessagePartList::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::MimeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::EncapsulatedRfc822MessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::HtmlMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderEncrypted(const MimeTreeParser::EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderSigned(const MimeTreeParser::SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::AlternativeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeTreeParser::CertMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    [[nodiscard]] bool renderWithFactory(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer) override;
    void renderFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter);

    [[nodiscard]] bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    [[nodiscard]] MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    [[nodiscard]] bool showEmoticons() const override;
    [[nodiscard]] bool isPrinting() const override;
    [[nodiscard]] bool htmlLoadExternal() const override;
    [[nodiscard]] bool showExpandQuotesMark() const override;
    [[nodiscard]] bool showOnlyOneMimePart() const override;
    [[nodiscard]] bool showSignatureDetails() const override;
    [[nodiscard]] bool showEncryptionDetails() const override;
    [[nodiscard]] int levelQuote() const override;
    MimeTreeParser::MessagePart::Ptr mMsgPart;
    CSSHelperBase *mCSSHelper = nullptr;
    const MessagePartRendererFactory *mRendererFactory = nullptr;
    const AttachmentStrategy *mAttachmentStrategy = nullptr;
    std::function<QString(KMime::Message *)> mCreateMessageHeader;

    int mLevelQuote = 1;

    bool mShowEmoticons = false;
    bool mIsPrinting = false;
    bool mHtmlLoadExternal = false;
    bool mShowExpandQuotesMark = false;
    bool mShowOnlyOneMimePart = false;
    bool mShowSignatureDetails = false;
    bool mShowEncryptionDetails = false;
};
}
