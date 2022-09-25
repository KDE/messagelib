/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "texthtml.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

using namespace MimeTreeParser;

const TextHtmlBodyPartFormatter *TextHtmlBodyPartFormatter::self;

const Interface::BodyPartFormatter *TextHtmlBodyPartFormatter::create()
{
    if (!self) {
        self = new TextHtmlBodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr TextHtmlBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    HtmlMessagePart::Ptr mp(new HtmlMessagePart(part.objectTreeParser(), node, part.source()));
    return mp;
}
