/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartmixed.h"

#include "messagepart.h"
#include "multipartencrypted.h"
#include "objecttreeparser.h"

#include <KMime/Content>

using namespace MimeTreeParser;

const MultiPartMixedBodyPartFormatter *MultiPartMixedBodyPartFormatter::self;

namespace
{
bool partHasMimeType(KMime::Content *part, const char *mt)
{
    const auto ct = part->contentType(false);
    return ct && ct->isMimeType(mt);
}
}

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

    auto contents = part.content()->contents();

    // handling of gpupg wks is in kdepim-addons
    if (contents.size() == 2 && partHasMimeType(contents[0], "text/plain") && partHasMimeType(contents[1], "application/vnd.gnupg.wks")) {
        return MimeMessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), contents.at(1), false));
    }

    for (const auto &content : std::as_const(contents)) {
        if (partHasMimeType(content, "application/pgp-encrypted")) {
            // Remove explaination if any
            for (const auto &content : std::as_const(contents)) {
                if (content->hasHeader("X-PGP-HIDDEN")) {
                    part.content()->takeContent(content);
                }
            }

            return MultiPartEncryptedBodyPartFormatter::create()->process(part);
        }
    }

    // normal treatment of the parts in the mp/mixed container
    MimeMessagePart::Ptr mp(new MimeMessagePart(part.objectTreeParser(), contents.at(0), false));
    return mp;
}
