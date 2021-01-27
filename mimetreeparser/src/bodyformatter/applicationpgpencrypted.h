/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_APPLICATIONPGPENCRYPTED_H
#define MIMETREEPARSER_BODYFORAMATTER_APPLICATIONPGPENCRYPTED_H

#include "interfaces/bodypart.h"
#include "interfaces/bodypartformatter.h"

namespace MimeTreeParser
{
class ApplicationPGPEncryptedBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const ApplicationPGPEncryptedBodyPartFormatter *self;

public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

#endif
