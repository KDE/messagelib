/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "textplain.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

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

    TextMessagePart::Ptr mp;
    if (isFirstTextPart) {
        mp = TextMessagePart::Ptr(new TextMessagePart(part.objectTreeParser(), node, part.source()->decryptMessage()));
    } else {
        mp = TextMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, part.source()->decryptMessage()));
    }

    part.processResult()->setInlineSignatureState(mp->signatureState());
    part.processResult()->setInlineEncryptionState(mp->encryptionState());

    part.nodeHelper()->setNodeDisplayedEmbedded(node, true);

    return mp;
}
