/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepartrenderer.h"

#include "quotehtml.h"

#include "../htmlblock.h"

#include "htmlwriter/bufferedhtmlwriter.h"
#include "interfaces/htmlwriter.h"

using namespace MessageViewer;
using namespace Qt::StringLiterals;

MessagePartRenderer::MessagePartRenderer() = default;

MessagePartRenderer::~MessagePartRenderer() = default;

bool MessagePartRenderer::render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const
{
    AttachmentMarkBlock block(nullptr, msgPart->attachmentContent());
    if (msgPart->isAttachment()) {
        htmlWriter->write(block.enter());
    }
    BufferedHtmlWriter bufferedWriter;
    bufferedWriter.begin();
    quotedHTML(msgPart->text(), context, &bufferedWriter);
    bufferedWriter.end();

    const auto content = bufferedWriter.data();
    const QString iframeContent =
        (u"<iframe style=\"width: 100%\" src=\"about:blank\" data-content=\""_s + QString::fromUtf8(content).toHtmlEscaped() + u"\"></iframe>"_s);
    htmlWriter->write(iframeContent);
    htmlWriter->write(block.exit());
    return true;
}
