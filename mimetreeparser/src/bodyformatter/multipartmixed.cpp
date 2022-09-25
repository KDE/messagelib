/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartmixed.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

using namespace MimeTreeParser;

const MultiPartMixedBodyPartFormatter *MultiPartMixedBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartMixedBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartMixedBodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr MultiPartMixedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    if (part.content()->contents().isEmpty()) {
        return {};
    }

    // normal treatment of the parts in the mp/mixed container
    MimeMessagePart::Ptr mp(new MimeMessagePart(part.objectTreeParser(), part.content()->contents().at(0), false));
    return mp;
}
