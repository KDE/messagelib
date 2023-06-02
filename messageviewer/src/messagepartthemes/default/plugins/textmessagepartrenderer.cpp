/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textmessagepartrenderer.h"

#include "../messagepartrenderermanager.h"

#include "interfaces/htmlwriter.h"
#include <KTextTemplate/Context>
#include <KTextTemplate/Template>
#include <MessageCore/StringUtil>

using namespace MessageViewer;

TextMessagePartRenderer::TextMessagePartRenderer() = default;

TextMessagePartRenderer::~TextMessagePartRenderer() = default;

bool TextMessagePartRenderer::render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const
{
    Q_UNUSED(context)
    auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
    if (!mp) {
        return false;
    }

    if (context->isHiddenHint(msgPart)) {
        return true;
    }

    if (context->displayHint(msgPart) != MimeTreeParser::NoIcon) {
        return context->renderWithFactory<MimeTreeParser::AttachmentMessagePart>(mp, htmlWriter);
    }
    KTextTemplate::Template t;
    KTextTemplate::Context c = MessagePartRendererManager::self()->createContext();
    c.insert(QStringLiteral("block"), msgPart.data());
    c.insert(QStringLiteral("showOnlyOneMimePart"), context->showOnlyOneMimePart());
    c.insert(QStringLiteral("content"), QVariant::fromValue<KTextTemplateCallback>([mp, htmlWriter, context](KTextTemplate::OutputStream *) {
                 context->renderSubParts(mp, htmlWriter);
             }));

    t = MessagePartRendererManager::self()->loadByName(QStringLiteral("textmessagepart.html"));
    KTextTemplate::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
    return true;
}
