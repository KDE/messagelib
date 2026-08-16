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
public:
    [[nodiscard]] MessagePartPtr process(Interface::BodyPart &part) const override;

private:
    [[nodiscard]] bool isMailmanMessage(KMime::Content *curNode) const;
};
}
