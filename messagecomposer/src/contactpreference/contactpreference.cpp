/*
   SPDX-FileCopyrightText: 2022 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "contactpreference.h"

#include <KContacts/Addressee>

using namespace MessageComposer;

ContactPreference::ContactPreference()
    : encryptionPreference(Kleo::UnknownPreference)
    , signingPreference(Kleo::UnknownSigningPreference)
    , cryptoMessageFormat(Kleo::AutoFormat)
{
}

void ContactPreference::fillFromAddressee(const KContacts::Addressee& contact)
{
    QString encryptPref = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOENCRYPTPREF"));
    encryptionPreference = Kleo::stringToEncryptionPreference(encryptPref);
    QString signPref = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOSIGNPREF"));
    signingPreference = Kleo::stringToSigningPreference(signPref);
    QString cryptoFormats = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("CRYPTOPROTOPREF"));
    cryptoMessageFormat = Kleo::stringToCryptoMessageFormat(cryptoFormats);
    pgpKeyFingerprints = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("OPENPGPFP")).split(QLatin1Char(','), Qt::SkipEmptyParts);
    smimeCertFingerprints = contact.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("SMIMEFP")).split(QLatin1Char(','), Qt::SkipEmptyParts);
}

void ContactPreference::fillAddressee(KContacts::Addressee& contact) const
{
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOENCRYPTPREF"),
                         QLatin1String(Kleo::encryptionPreferenceToString(encryptionPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOSIGNPREF"),
                         QLatin1String(Kleo::signingPreferenceToString(signingPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOPROTOPREF"),
                         QLatin1String(cryptoMessageFormatToString(cryptoMessageFormat)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("OPENPGPFP"), pgpKeyFingerprints.join(QLatin1Char(',')));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("SMIMEFP"), smimeCertFingerprints.join(QLatin1Char(',')));
}
