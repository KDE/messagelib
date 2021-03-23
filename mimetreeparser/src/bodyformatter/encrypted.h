/*
   SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "interfaces/bodypart.h"
#include "interfaces/bodypartformatter.h"

namespace MimeTreeParser
{
class EncryptedBodyPartFormatter : public Interface::BodyPartFormatter
{
    static const EncryptedBodyPartFormatter *self;

public:
    MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create();
};
}

