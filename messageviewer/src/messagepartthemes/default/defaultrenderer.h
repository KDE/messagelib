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

#ifndef MESSAGEVIEWER_DEFAULTRENDERER_H
#define MESSAGEVIEWER_DEFAULTRENDERER_H

#include <QSharedPointer>

#include <functional>

namespace KMime {
class Message;
}

namespace MimeTreeParser {
class MessagePart;
typedef QSharedPointer<MessagePart> MessagePartPtr;
}

namespace MessageViewer {
class DefaultRendererPrivate;
class HtmlWriter;
class AttachmentStrategy;
class CSSHelperBase;

class DefaultRenderer
{
public:
    explicit DefaultRenderer(CSSHelperBase *cssHelder);
    ~DefaultRenderer();

    void setShowOnlyOneMimePart(bool onlyOneMimePart);
    void setAttachmentStrategy(const AttachmentStrategy *strategy);
    void setShowEmoticons(bool showEmoticons);
    void setIsPrinting(bool isPrinting);
    void setShowExpandQuotesMark(bool showExpandQuotesMark);
    void setShowSignatureDetails(bool showSignatureDetails);
    void setLevelQuote(int levelQuote);
    void setHtmlLoadExternal(bool htmlLoadExternal);
    void setCreateMessageHeader(std::function<QString(KMime::Message *)>);
    void render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *writer);
    void setShowEncryptionDetails(bool showEncryptionDetails);
private:
    DefaultRendererPrivate *d;
};
}
#endif //__MESSAGEVIEWER_MAILRENDERER_H
