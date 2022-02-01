/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textmessagepartrenderer.h"

#include "quotehtml.h"

#include "../messagepartrenderermanager.h"

#include "interfaces/htmlwriter.h"
#include <MessageCore/StringUtil>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <grantlee/context.h>
#include <grantlee/template.h>
#else
#include <KTextTemplate/context.h>
#include <KTextTemplate/template.h>
#endif

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

    const auto tmpAsIcon = context->displayHint(msgPart);
    if (tmpAsIcon != MimeTreeParser::NoIcon) {
        return context->renderWithFactory<MimeTreeParser::AttachmentMessagePart>(mp, htmlWriter);
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::Template t;
    Grantlee::Context c = MessagePartRendererManager::self()->createContext();
#else
    KTextTemplate::Template t;
    KTextTemplate::Context c = MessagePartRendererManager::self()->createContext();
#endif
    c.insert(QStringLiteral("block"), msgPart.data());
    c.insert(QStringLiteral("showOnlyOneMimePart"), context->showOnlyOneMimePart());
    c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([mp, htmlWriter, context](Grantlee::OutputStream *) {
                 context->renderSubParts(mp, htmlWriter);
             }));
    t = MessagePartRendererManager::self()->loadByName(QStringLiteral("textmessagepart.html"));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::OutputStream s(htmlWriter->stream());
#else
    KTextTemplate::OutputStream s(htmlWriter->stream());
#endif
    t->render(&s, &c);
    return true;
}
