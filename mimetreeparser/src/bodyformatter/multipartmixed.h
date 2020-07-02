/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_MULTIPARTMIXED_H
#define MIMETREEPARSER_BODYFORAMATTER_MULTIPARTMIXED_H

#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"

namespace MimeTreeParser {
class MultiPartMixedBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const MultiPartMixedBodyPartFormatter *self;
public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

#endif
