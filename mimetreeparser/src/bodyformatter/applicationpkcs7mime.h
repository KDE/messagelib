/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_APPLICATIONPKCS7MIME_H
#define MIMETREEPARSER_BODYFORAMATTER_APPLICATIONPKCS7MIME_H

#include "interfaces/bodypart.h"
#include "interfaces/bodypartformatter.h"

namespace MimeTreeParser
{
class ApplicationPkcs7MimeBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const ApplicationPkcs7MimeBodyPartFormatter *self;

public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

#endif
