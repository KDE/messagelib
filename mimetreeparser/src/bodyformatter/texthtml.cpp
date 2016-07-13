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

#include "texthtml.h"

#include "viewer/attachmentstrategy.h"
#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const TextHtmlBodyPartFormatter *TextHtmlBodyPartFormatter::self;

const Interface::BodyPartFormatter *TextHtmlBodyPartFormatter::create()
{
    if (!self) {
        self = new TextHtmlBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result TextHtmlBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr TextHtmlBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    HtmlMessagePart::Ptr mp(new HtmlMessagePart(part.objectTreeParser(), node, part.source()));
    return mp;
}
