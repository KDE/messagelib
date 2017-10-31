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

#include "textmessagepartrenderer.h"

#include "quotehtml.h"

#include "../defaultrenderer_p.h"
#include "../messagepartrenderermanager.h"

#include <MessageCore/StringUtil>
#include "interfaces/htmlwriter.h"

#include <grantlee/context.h>
#include <grantlee/template.h>

using namespace MessageViewer;

TextMessagePartRenderer::TextMessagePartRenderer()
{
}

TextMessagePartRenderer::~TextMessagePartRenderer()
{
}

bool TextMessagePartRenderer::render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const
{
    Q_UNUSED(context);
    auto mp = msgPart.dynamicCast<TextMessagePart>();
    if (!mp) {
        return false;
    }

    if (context->isHiddenHint(msgPart)) {
        return true;
    }

    Grantlee::Template t;
    Grantlee::Context c = MessagePartRendererManager::self()->createContext();
    c.insert(QStringLiteral("block"), msgPart.data());
    c.insert(QStringLiteral("content"), QVariant::fromValue<GrantleeCallback>([mp, htmlWriter, context](Grantlee::OutputStream *) {
        context->renderSubParts(mp, htmlWriter);
    }));
    t = MessagePartRendererManager::self()->loadByName(QStringLiteral(":/textmessagepart.html"));

    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
    return true;
}
