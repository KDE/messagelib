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
/*! \class ContactPreference
    \inheaderfile contactpreference.h
    \brief Represents cryptographic preferences for a contact.
    \inmodule MessageComposer

    This class stores cryptographic preferences (encryption and signing) for a specific contact,
    including the preferred crypto message format and key fingerprints for PGP and S/MIME.
*/
class MESSAGECOMPOSER_EXPORT ContactPreference
{
public:
    /*! \brief Constructs an empty ContactPreference. */
    ContactPreference();

    /*! \brief Fills the preference data from a KContacts contact. */
    void fillFromAddressee(const KContacts::Addressee &contact);

    /*! \brief Applies the preference data to a KContacts contact. */
    void fillAddressee(KContacts::Addressee &contact) const;

    Kleo::EncryptionPreference encryptionPreference = Kleo::UnknownPreference;
    Kleo::SigningPreference signingPreference = Kleo::UnknownSigningPreference;
    Kleo::CryptoMessageFormat cryptoMessageFormat = Kleo::AutoFormat;
    QStringList pgpKeyFingerprints;
    QStringList smimeCertFingerprints;
};
}
