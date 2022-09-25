/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagepartrenderer.h"

#include "quotehtml.h"

#include "../htmlblock.h"

#include "interfaces/htmlwriter.h"

using namespace MessageViewer;

MessagePartRenderer::MessagePartRenderer() = default;

MessagePartRenderer::~MessagePartRenderer() = default;

bool MessagePartRenderer::render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const
{
    AttachmentMarkBlock block(nullptr, msgPart->attachmentContent());
    if (msgPart->isAttachment()) {
        htmlWriter->write(block.enter());
    }
    quotedHTML(msgPart->text(), context, htmlWriter);
    htmlWriter->write(block.exit());
    return true;
}
