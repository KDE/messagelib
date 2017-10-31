/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This library is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
   License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
#ifndef __MESSAGEVIEWER_DEFAULTRENDERER_P_H__
#define __MESSAGEVIEWER_DEFAULTRENDERER_P_H__

#include "defaultrenderer.h"
#include "messagepartrendererbase.h"

#include <MimeTreeParser/MessagePart>

using namespace MimeTreeParser;
using namespace MessageViewer;

namespace MessageViewer {
class AttachmentStrategy;
class MessagePartRendererFactory;
class CSSHelperBase;
class HtmlWriter;
}

namespace MimeTreeParser {

class DefaultRendererPrivate : public MessageViewer::RenderContext
{
public:
    DefaultRendererPrivate(CSSHelperBase *cssHelper, const MessagePartRendererFactory *rendererFactory);
    ~DefaultRendererPrivate();

    CSSHelperBase *cssHelper() const override;

    Interface::ObjectTreeSource *source() const;

    void renderSubParts(const MessagePart::Ptr &msgPart, MessageViewer::HtmlWriter *htmlWriter) override;

    void render(const MessagePartList::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const MimeMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const EncapsulatedRfc822MessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const HtmlMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void renderEncrypted(const EncryptedMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void renderSigned(const SignedMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const SignedMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const EncryptedMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const AlternativeMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    void render(const CertMessagePart::Ptr &mp, MessageViewer::HtmlWriter *htmlWriter);
    bool renderWithFactory(const QMetaObject *mo, const MessagePart::Ptr &msgPart, MessageViewer::HtmlWriter *writer) override;
    using RenderContext::renderWithFactory;
    void renderFactory(const MessagePart::Ptr &msgPart, MessageViewer::HtmlWriter *htmlWriter);

    bool isHiddenHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;
    MimeTreeParser::IconType displayHint(const MimeTreeParser::MessagePart::Ptr &msgPart) override;

    bool mShowOnlyOneMimePart = false;
    MessagePart::Ptr mMsgPart;
    CSSHelperBase *mCSSHelper = nullptr;
    const MessageViewer::MessagePartRendererFactory *mRendererFactory = nullptr;
    const MessageViewer::AttachmentStrategy *mAttachmentStrategy = nullptr;
};
}
#endif
