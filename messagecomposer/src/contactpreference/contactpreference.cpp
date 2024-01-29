/*
   SPDX-FileCopyrightText: 2022 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "contactpreference.h"

#include <KContacts/Addressee>

using namespace MessageComposer;

ContactPreference::ContactPreference() = default;

void ContactPreference::fillFromAddressee(const KContacts::Addressee &contact)
{
    const QString encryptPref = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOENCRYPTPREF"));
    encryptionPreference = Kleo::stringToEncryptionPreference(encryptPref);
    const QString signPref = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOSIGNPREF"));
    signingPreference = Kleo::stringToSigningPreference(signPref);
    const QString cryptoFormats = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOPROTOPREF"));
    cryptoMessageFormat = Kleo::stringToCryptoMessageFormat(cryptoFormats);
    pgpKeyFingerprints = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("OPENPGPFP")).split(QLatin1Char(','), Qt::SkipEmptyParts);
    smimeCertFingerprints = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("SMIMEFP")).split(QLatin1Char(','), Qt::SkipEmptyParts);
}

void ContactPreference::fillAddressee(KContacts::Addressee &contact) const
{
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOENCRYPTPREF"),
                         QLatin1StringView(Kleo::encryptionPreferenceToString(encryptionPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOSIGNPREF"),
                         QLatin1StringView(Kleo::signingPreferenceToString(signingPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOPROTOPREF"),
                         QLatin1StringView(cryptoMessageFormatToString(cryptoMessageFormat)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("OPENPGPFP"), pgpKeyFingerprints.join(QLatin1Char(',')));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("SMIMEFP"), smimeCertFingerprints.join(QLatin1Char(',')));
}
