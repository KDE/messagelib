/*
   SPDX-FileCopyrightText: 2022 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Libkleo/Enum>

#include "messagecomposer_export.h"

namespace KContacts
{
class Addressee;
}

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT ContactPreference
{
public:
    ContactPreference();

    void fillFromAddressee(const KContacts::Addressee &contact);
    void fillAddressee(KContacts::Addressee &contact) const;

    Kleo::EncryptionPreference encryptionPreference = Kleo::UnknownPreference;
    Kleo::SigningPreference signingPreference = Kleo::UnknownSigningPreference;
    Kleo::CryptoMessageFormat cryptoMessageFormat = Kleo::AutoFormat;
    QStringList pgpKeyFingerprints;
    QStringList smimeCertFingerprints;
};
}
