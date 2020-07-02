/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MIMETREEPARSER_BODYFORAMATTER_MAILMAN_H
#define MIMETREEPARSER_BODYFORAMATTER_MAILMAN_H

#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"

namespace MimeTreeParser {
class MailmanBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const MailmanBodyPartFormatter *self;
public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();

private:
    bool isMailmanMessage(KMime::Content *curNode) const;
};
}

#endif
