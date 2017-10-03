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

class PartRendered;

namespace MessageViewer {
class MessagePartRendererFactory;
class CSSHelperBase;
class HtmlWriter;
}

namespace MimeTreeParser {
class DefaultRendererPrivate : public MessageViewer::RenderContext
{
public:
    DefaultRendererPrivate(const MessagePart::Ptr &msgPart, CSSHelperBase *cssHelper, HtmlWriter *writer, const MessagePartRendererFactory *rendererFactory);
    ~DefaultRendererPrivate();

    CSSHelperBase *cssHelper() const override;

    Interface::ObjectTreeSource *source() const;

    void renderSubParts(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter) override;

    void render(const MessagePartList::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const MimeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const EncapsulatedRfc822MessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const HtmlMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderEncrypted(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void renderSigned(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const SignedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const EncryptedMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const AlternativeMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    void render(const CertMessagePart::Ptr &mp, HtmlWriter *htmlWriter);
    bool renderWithFactory(const QString &className, const MessagePart::Ptr &msgPart, HtmlWriter *writer) override;
    void renderFactory(const MessagePart::Ptr &msgPart, HtmlWriter *htmlWriter);

    MessagePart::Ptr mMsgPart;
    CSSHelperBase *mCSSHelper = nullptr;
    const MessageViewer::MessagePartRendererFactory *mRendererFactory = nullptr;
};
}
#endif
