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

#include "textplain.h"

#include "viewer/attachmentstrategy.h"
#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const TextPlainBodyPartFormatter *TextPlainBodyPartFormatter::self;

const Interface::BodyPartFormatter *TextPlainBodyPartFormatter::create()
{
    if (!self) {
        self = new TextPlainBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result TextPlainBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
{
    Q_UNUSED(writer)
    const auto p = process(*part);
    const auto mp = static_cast<MessagePart *>(p.data());
    if (mp) {
        mp->html(false);
        return Ok;
    }
    return Failed;
}

Interface::MessagePart::Ptr TextPlainBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    const bool isFirstTextPart = (node->topLevel()->textContent() == node);

    IconType iconType = NoIcon;
    if (!isFirstTextPart && part.objectTreeParser()->attachmentStrategy()->defaultDisplay(node) != AttachmentStrategy::Inline &&
            !part.objectTreeParser()->showOnlyOneMimePart()) {
        iconType = MimeTreeParser::IconExternal;
    }

    if (!isFirstTextPart && (part.objectTreeParser()->attachmentStrategy()->defaultDisplay(node) == AttachmentStrategy::None) && !part.objectTreeParser()->showOnlyOneMimePart()) {
        part.nodeHelper()->setNodeDisplayedHidden(node, true);
    }

    part.objectTreeParser()->extractNodeInfos(node, isFirstTextPart);

    QString label = NodeHelper::fileName(node);

    const bool bDrawFrame = !isFirstTextPart
                            && !part.objectTreeParser()->showOnlyOneMimePart()
                            && !label.isEmpty()
                            && iconType == MimeTreeParser::NoIcon;
    const QString fileName = part.nodeHelper()->writeNodeToTempFile(node);

    TextMessagePart::Ptr mp(new TextMessagePart(part.objectTreeParser(), node, bDrawFrame, !fileName.isEmpty(), part.source()->decryptMessage(), iconType));

    part.processResult()->setInlineSignatureState(mp->signatureState());
    part.processResult()->setInlineEncryptionState(mp->encryptionState());

    if (isFirstTextPart) {
        part.objectTreeParser()->setPlainTextContent(mp->text());
    }

    part.nodeHelper()->setNodeDisplayedEmbedded(node, true);

    return mp;
}
