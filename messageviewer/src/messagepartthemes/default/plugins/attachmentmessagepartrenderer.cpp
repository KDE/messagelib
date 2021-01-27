/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentmessagepartrenderer.h"

#include "quotehtml.h"

#include "../messagepartrenderermanager.h"
#include "utils/mimetype.h"

#include "interfaces/htmlwriter.h"
#include <MessageCore/StringUtil>

#include <KIconLoader>
#include <QUrl>

using namespace MessageViewer;

AttachmentMessagePartRenderer::AttachmentMessagePartRenderer()
{
}

AttachmentMessagePartRenderer::~AttachmentMessagePartRenderer()
{
}

bool AttachmentMessagePartRenderer::render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const
{
    auto mp = msgPart.dynamicCast<MimeTreeParser::TextMessagePart>();
    if (!mp) {
        return false;
    }

    KMime::Content *node = mp->content();
    if (context->isHiddenHint(msgPart)) {
        return true;
    }

    const auto tmpAsIcon = context->displayHint(msgPart);

    if (tmpAsIcon == MimeTreeParser::NoIcon) {
        return context->renderWithFactory<MimeTreeParser::TextMessagePart>(mp, htmlWriter);
    }

    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("asiconpart.html"));
    Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    c.insert(QStringLiteral("block"), msgPart.data());

    msgPart->setProperty("inline", (tmpAsIcon == MimeTreeParser::IconInline));

    QString iconPath;
    if (tmpAsIcon == MimeTreeParser::IconInline) {
        iconPath = mp->temporaryFilePath();
    } else {
        iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
        if (iconPath.right(14) == QLatin1String("mime_empty.png")) {
            MimeTreeParser::NodeHelper::magicSetType(node);
            iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
        }
    }
    msgPart->setProperty("iconPath", QUrl::fromLocalFile(iconPath).url());

    Grantlee::OutputStream s(htmlWriter->stream());
    t->render(&s, &c);
    return true;
}
