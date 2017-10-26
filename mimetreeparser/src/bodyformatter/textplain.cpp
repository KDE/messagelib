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

#include "attachmentstrategy.h"
#include "objecttreeparser.h"
#include "messagepart.h"

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

MessagePart::Ptr TextPlainBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    const bool isFirstTextPart = (node->topLevel()->textContent() == node);

    QString label = NodeHelper::fileName(node);

    const bool bDrawFrame = !isFirstTextPart
                            && !part.objectTreeParser()->showOnlyOneMimePart()
                            && !label.isEmpty();

    TextMessagePart::Ptr mp;
    if (isFirstTextPart) {
        mp = TextMessagePart::Ptr(new TextMessagePart(part.objectTreeParser(), node, bDrawFrame, part.source()->decryptMessage()));
    } else {
        mp = TextMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, bDrawFrame, part.source()->decryptMessage()));
    }

    part.processResult()->setInlineSignatureState(mp->signatureState());
    part.processResult()->setInlineEncryptionState(mp->encryptionState());

    part.nodeHelper()->setNodeDisplayedEmbedded(node, true);

    return mp;
}
