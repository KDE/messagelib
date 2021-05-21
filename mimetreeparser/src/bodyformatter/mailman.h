/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "interfaces/bodypart.h"
#include "interfaces/bodypartformatter.h"

namespace MimeTreeParser
{
class MailmanBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const MailmanBodyPartFormatter *self;

public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();

private:
    Q_REQUIRED_RESULT bool isMailmanMessage(KMime::Content *curNode) const;
};
}

