/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include <functional>

namespace KMime
{
class Message;
}

namespace MimeTreeParser
{
class MessagePart;
using MessagePartPtr = QSharedPointer<MessagePart>;
}

namespace MessageViewer
{
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
    void setCreateMessageHeader(const std::function<QString(KMime::Message *)> &);
    void render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *writer);
    void setShowEncryptionDetails(bool showEncryptionDetails);

private:
    std::unique_ptr<DefaultRendererPrivate> const d;
};
}
