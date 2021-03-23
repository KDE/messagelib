/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "defaultrenderer.h"
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
    Q_REQUIRED_RESULT bool renderWithFactory(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *writer) override;
    void renderFactory(const MimeTreeParser::MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter);

    Q_REQUIRED_RESULT bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    Q_REQUIRED_RESULT MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    Q_REQUIRED_RESULT bool showEmoticons() const override;
    Q_REQUIRED_RESULT bool isPrinting() const override;
    Q_REQUIRED_RESULT bool htmlLoadExternal() const override;
    Q_REQUIRED_RESULT bool showExpandQuotesMark() const override;
    Q_REQUIRED_RESULT bool showOnlyOneMimePart() const override;
    Q_REQUIRED_RESULT bool showSignatureDetails() const override;
    Q_REQUIRED_RESULT bool showEncryptionDetails() const override;
    Q_REQUIRED_RESULT int levelQuote() const override;
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
