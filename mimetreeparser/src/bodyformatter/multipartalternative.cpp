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

#include "multipartalternative.h"

#include "utils.h"

#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <MessageCore/NodeHelper>

#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const MultiPartAlternativeBodyPartFormatter *MultiPartAlternativeBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartAlternativeBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartAlternativeBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result MultiPartAlternativeBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr MultiPartAlternativeBodyPartFormatter::process(Interface::BodyPart &part) const
{
    const KMime::Content *node = part.content();
    KMime::Content *child = MessageCore::NodeHelper::firstChild(node);
    if (!child) {
        return MessagePart::Ptr();
    }

    KMime::Content *dataHtml = findType(child, "text/html", false, true);
    KMime::Content *dataPlain = findType(child, "text/plain", false, true);

    if (!dataHtml) {
        // If we didn't find the HTML part as the first child of the multipart/alternative, it might
        // be that this is a HTML message with images, and text/plain and multipart/related are the
        // immediate children of this multipart/alternative node.
        // In this case, the HTML node is a child of multipart/related.
        dataHtml = findType(child, "multipart/related", false, true);

        // Still not found? Stupid apple mail actually puts the attachments inside of the
        // multipart/alternative, which is wrong. Therefore we also have to look for multipart/mixed
        // here.
        // Do this only when prefering HTML mail, though, since otherwise the attachments are hidden
        // when displaying plain text.
        if (!dataHtml && part.source()->htmlMail()) {
            dataHtml = findType(child, "multipart/mixed", false, true);
        }
    }

    if (dataPlain || dataHtml) {
        AlternativeMessagePart::Ptr mp(new AlternativeMessagePart(part.objectTreeParser(), dataPlain, dataHtml));

        if ((part.source()->htmlMail() && dataHtml) ||
                (dataHtml && dataPlain && dataPlain->body().isEmpty())) {
            if (dataPlain) {
                part.nodeHelper()->setNodeProcessed(dataPlain, false);
            }
            part.source()->setHtmlMode(Util::MultipartHtml);
            mp->setViewHtml(true);
        }

        if (!part.source()->htmlMail() && dataPlain) {
            part.nodeHelper()->setNodeProcessed(dataHtml, false);
            part.source()->setHtmlMode(Util::MultipartPlain);
            mp->setViewHtml(false);
        }
        return mp;
    }

    MimeMessagePart::Ptr mp(new MimeMessagePart(part.objectTreeParser(), child, false));
    return mp;
}
