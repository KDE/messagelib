/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "attachmentmessagepartrenderer.h"

#include "../messagepartrenderermanager.h"
#include "utils/mimetype.h"

#include "interfaces/htmlwriter.h"
#include <MessageCore/StringUtil>

#include <KIconLoader>
#include <QUrl>

using namespace MessageViewer;

AttachmentMessagePartRenderer::AttachmentMessagePartRenderer() = default;

AttachmentMessagePartRenderer::~AttachmentMessagePartRenderer() = default;

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

    KTextTemplate::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral("asiconpart.html"));
    KTextTemplate::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Grantlee::OutputStream s(htmlWriter->stream());
#else
    KTextTemplate::OutputStream s(htmlWriter->stream());
#endif
    t->render(&s, &c);
    return true;
}
