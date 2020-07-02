/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_TEXTHTML_H
#define MIMETREEPARSER_BODYFORAMATTER_TEXTHTML_H

#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"

namespace MimeTreeParser {
class TextHtmlBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const TextHtmlBodyPartFormatter *self;
public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

#endif
