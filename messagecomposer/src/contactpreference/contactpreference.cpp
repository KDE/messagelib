/*
   SPDX-FileCopyrightText: 2022 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "contactpreference.h"

#include <KContacts/Addressee>

using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

ContactPreference::ContactPreference() = default;

void ContactPreference::fillFromAddressee(const KContacts::Addressee &contact)
{
    const QString encryptPref = contact.custom(u"KADDRESSBOOK"_s, u"CRYPTOENCRYPTPREF"_s);
    encryptionPreference = Kleo::stringToEncryptionPreference(encryptPref);
    const QString signPref = contact.custom(u"KADDRESSBOOK"_s, u"CRYPTOSIGNPREF"_s);
    signingPreference = Kleo::stringToSigningPreference(signPref);
    const QString cryptoFormats = contact.custom(u"KADDRESSBOOK"_s, u"CRYPTOPROTOPREF"_s);
    cryptoMessageFormat = Kleo::stringToCryptoMessageFormat(cryptoFormats);
    pgpKeyFingerprints = contact.custom(u"KADDRESSBOOK"_s, u"OPENPGPFP"_s).split(u',', Qt::SkipEmptyParts);
    smimeCertFingerprints = contact.custom(u"KADDRESSBOOK"_s, u"SMIMEFP"_s).split(u',', Qt::SkipEmptyParts);
}

void ContactPreference::fillAddressee(KContacts::Addressee &contact) const
{
    contact.insertCustom(u"KADDRESSBOOK"_s, u"CRYPTOENCRYPTPREF"_s, QLatin1StringView(Kleo::encryptionPreferenceToString(encryptionPreference)));
    contact.insertCustom(u"KADDRESSBOOK"_s, u"CRYPTOSIGNPREF"_s, QLatin1StringView(Kleo::signingPreferenceToString(signingPreference)));
    contact.insertCustom(u"KADDRESSBOOK"_s, u"CRYPTOPROTOPREF"_s, QLatin1StringView(cryptoMessageFormatToString(cryptoMessageFormat)));
    contact.insertCustom(u"KADDRESSBOOK"_s, u"OPENPGPFP"_s, pgpKeyFingerprints.join(u','));
    contact.insertCustom(u"KADDRESSBOOK"_s, u"SMIMEFP"_s, smimeCertFingerprints.join(u','));
}
