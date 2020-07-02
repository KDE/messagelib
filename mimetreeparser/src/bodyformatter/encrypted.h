/*
   SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_ENCRYPTED_H
#define MIMETREEPARSER_BODYFORAMATTER_ENCRYPTED_H

#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"

namespace MimeTreeParser {
class EncryptedBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const EncryptedBodyPartFormatter *self;
public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

#endif
