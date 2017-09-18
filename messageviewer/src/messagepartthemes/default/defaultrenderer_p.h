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

#include <MimeTreeParser/MessagePart>

using namespace MimeTreeParser;
using namespace MessageViewer;

class CacheHtmlWriter;
class PartRendered;

namespace MessageViewer {
class MessagePartRendererFactoryBase;
class CSSHelperBase;
}

namespace MimeTreeParser {
class DefaultRendererPrivate
{
public:
    DefaultRendererPrivate(const Interface::MessagePart::Ptr &msgPart, CSSHelperBase *cssHelper, const MessagePartRendererFactoryBase *rendererFactory);
    ~DefaultRendererPrivate();

    QString alignText();
    CSSHelperBase *cssHelper() const;

    Interface::ObjectTreeSource *source() const;

    void renderSubParts(const MessagePart::Ptr &msgPart, const QSharedPointer<CacheHtmlWriter> &htmlWriter);

    QString render(const MessagePartList::Ptr &mp);
    QString render(const MimeMessagePart::Ptr &mp);
    QString render(const EncapsulatedRfc822MessagePart::Ptr &mp);
    QString render(const HtmlMessagePart::Ptr &mp);
    QString renderEncrypted(const EncryptedMessagePart::Ptr &mp);
    QString renderSigned(const SignedMessagePart::Ptr &mp);
    QString render(const SignedMessagePart::Ptr &mp);
    QString render(const EncryptedMessagePart::Ptr &mp);
    QString render(const AlternativeMessagePart::Ptr &mp);
    QString render(const CertMessagePart::Ptr &mp);
    QSharedPointer<PartRendered> renderWithFactory(QString className, const Interface::MessagePart::Ptr &msgPart);
    QString renderFactory(const Interface::MessagePart::Ptr &msgPart, const QSharedPointer<CacheHtmlWriter> &htmlWriter);

    QString mHtml;
    Interface::MessagePart::Ptr mMsgPart;
    HtmlWriter *mOldWriter = nullptr;

    CSSHelperBase *mCSSHelper = nullptr;
    const MessageViewer::MessagePartRendererFactoryBase *mRendererFactory = nullptr;
};
}
#endif
