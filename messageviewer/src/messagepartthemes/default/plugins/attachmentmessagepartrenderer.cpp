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

#include "attachmentmessagepartrenderer.h"

#include "quotehtml.h"

#include "utils/mimetype.h"
#include "../partrendered.h"
#include "../defaultrenderer_p.h"
#include "../messagepartrenderermanager.h"

#include <MessageCore/StringUtil>

#include <KIconLoader>
#include <KLocalizedString>
#include <QUrl>

AttachmentMessagePartRenderer::AttachmentMessagePartRenderer()
{
}

AttachmentMessagePartRenderer::~AttachmentMessagePartRenderer()
{
}

QSharedPointer<PartRendered> AttachmentMessagePartRenderer::render(DefaultRendererPrivate *drp, const MimeTreeParser::Interface::MessagePartPtr &msgPart) const
{
    auto mp = msgPart.dynamicCast<AttachmentMessagePart>();
    if (!mp) {
        return QSharedPointer<PartRendered>();
    }

    KMime::Content *node = mp->mNode;
    NodeHelper *nodeHelper = mp->mOtp->nodeHelper();

    if (mp->isHidden()) {
        return QSharedPointer<PartRendered>(new EmptyPartRendered());
    }

    const auto tmpAsIcon = mp->asIcon();

    if (tmpAsIcon == MimeTreeParser::NoIcon) {
        return drp->renderWithFactory(QStringLiteral("MimeTreeParser::TextMessagePart"), mp);
    }

    Grantlee::Template t = MessageViewer::MessagePartRendererManager::self()->loadByName(QStringLiteral(":/asiconpart.html"));
    Grantlee::Context c = MessageViewer::MessagePartRendererManager::self()->createContext();
    QObject block;
    c.insert(QStringLiteral("block"), &block);

    block.setProperty("showTextFrame", mp->showTextFrame());
    block.setProperty("label", MessageCore::StringUtil::quoteHtmlChars(NodeHelper::fileName(node), true));
    block.setProperty("comment", MessageCore::StringUtil::quoteHtmlChars(node->contentDescription()->asUnicodeString(), true));
    block.setProperty("link", nodeHelper->asHREF(node, QStringLiteral("body")));
    block.setProperty("showLink", mp->showLink());
    block.setProperty("dir", drp->alignText());
    block.setProperty("iconSize", MessageViewer::MessagePartRendererManager::self()->iconCurrentSize());
    block.setProperty("inline", (tmpAsIcon == MimeTreeParser::IconInline));

    QString iconPath;
    if (tmpAsIcon == MimeTreeParser::IconInline) {
        iconPath = nodeHelper->writeNodeToTempFile(node);
    } else {
        iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
        if (iconPath.right(14) == QLatin1String("mime_empty.png")) {
            nodeHelper->magicSetType(node);
            iconPath = MessageViewer::Util::iconPathForContent(node, KIconLoader::Desktop);
        }
    }
    block.setProperty("iconPath", QUrl::fromLocalFile(iconPath).url());

    const QString name = node->contentType()->name();
    QString label = name.isEmpty() ? NodeHelper::fileName(node) : name;
    QString comment = node->contentDescription()->asUnicodeString();

    if (label.isEmpty()) {
        label = i18nc("display name for an unnamed attachment", "Unnamed");
    }
    label = MessageCore::StringUtil::quoteHtmlChars(label, true);

    comment = MessageCore::StringUtil::quoteHtmlChars(comment, true);
    if (label == comment) {
        comment.clear();
    }

    block.setProperty("label", label);
    block.setProperty("comment", comment);

    const auto html = t->render(&c);
    return QSharedPointer<PartRendered>(new HtmlOnlyPartRendered(mp, html));
}
