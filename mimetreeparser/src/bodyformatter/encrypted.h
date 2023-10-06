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
public:
    enum EncryptionFlag {
        AutoPGP = 0x0, ///< Detect PGP data automatically
        ForcePGP = 0x1, ///< Always decode PGP data
    };
    Q_DECLARE_FLAGS(EncryptionFlags, EncryptionFlag)

    [[nodiscard]] MessagePartPtr process(Interface::BodyPart &part) const override;
    static const Interface::BodyPartFormatter *create(EncryptionFlags flags);

private:
    EncryptionFlags mFlags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EncryptedBodyPartFormatter::EncryptionFlags)
}
