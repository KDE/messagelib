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

#include "multipartmixed.h"

#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <MessageCore/NodeHelper>
#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const MultiPartMixedBodyPartFormatter *MultiPartMixedBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartMixedBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartMixedBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result MultiPartMixedBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr MultiPartMixedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *child = MessageCore::NodeHelper::firstChild(part.content());
    if (!child) {
        return MessagePart::Ptr();
    }

    // normal treatment of the parts in the mp/mixed container
    MimeMessagePart::Ptr mp(new MimeMessagePart(part.objectTreeParser(), child, false));
    return mp;
}
