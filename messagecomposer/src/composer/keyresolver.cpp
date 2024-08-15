/*  -*- c++ -*-
    keyresolver.cpp

    This file is part of libkleopatra, the KDE keymanagement library
    SPDX-FileCopyrightText: 2004 Klarälvdalens Datakonsult AB

    Based on kpgp.cpp
    Copyright (C) 2001,2002 the KPGP authors
    See file libkdenetwork/AUTHORS.kpgp for details

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "composer/keyresolver.h"

#include "contactpreference/savecontactpreferencejob.h"

#include "utils/kleo_util.h"
#include <KCursorSaver>

#include <KEmailAddress>

#include <Libkleo/Algorithm>
#include <Libkleo/Compliance>
#include <Libkleo/ExpiryChecker>
#include <Libkleo/KeySelectionDialog>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>

#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>

#include "messagecomposer_debug.h"
#include <Akonadi/ContactSearchJob>
#include <KLocalizedString>
#include <KMessageBox>

#include <MessageCore/AutocryptRecipient>
#include <MessageCore/AutocryptStorage>

#include <QPointer>

#include <algorithm>
#include <cassert>
#include <ctime>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>

//
// some predicates to be used in STL algorithms:
//

static inline bool EmptyKeyList(const Kleo::KeyApprovalDialog::Item &item)
{
    return item.keys.empty();
}

static inline QString ItemDotAddress(const Kleo::KeyResolver::Item &item)
{
    return item.address;
}

static inline bool ApprovalNeeded(const Kleo::KeyResolver::Item &item)
{
    bool approvalNeeded = item.pref == Kleo::NeverEncrypt || item.keys.empty();
    if (!approvalNeeded && Kleo::DeVSCompliance::isCompliant()) {
        approvalNeeded = !Kleo::all_of(item.keys, &Kleo::DeVSCompliance::keyIsCompliant);
    }
    return approvalNeeded;
}

static inline Kleo::KeyResolver::Item CopyKeysAndEncryptionPreferences(const Kleo::KeyResolver::Item &oldItem, const Kleo::KeyApprovalDialog::Item &newItem)
{
    return Kleo::KeyResolver::Item(oldItem.address, newItem.keys, newItem.pref, oldItem.signPref, oldItem.format);
}

static bool ValidOpenPGPEncryptionKey(const GpgME::Key &key)
{
    if (key.protocol() != GpgME::OpenPGP) {
        return false;
    }
    if (key.isRevoked()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "is revoked";
    }
    if (key.isExpired()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "is expired";
    }
    if (key.isDisabled()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "is disabled";
    }
    if (!key.canEncrypt()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "can't encrypt";
    }
    if (key.isRevoked() || key.isExpired() || key.isDisabled() || !key.canEncrypt()) {
        return false;
    }
    return true;
}

static bool ValidTrustedOpenPGPEncryptionKey(const GpgME::Key &key)
{
    if (!ValidOpenPGPEncryptionKey(key)) {
        return false;
    }
    const std::vector<GpgME::UserID> uids = key.userIDs();
    auto end(uids.end());
    for (auto it = uids.begin(); it != end; ++it) {
        if (!it->isRevoked() && it->validity() >= GpgME::UserID::Marginal) {
            return true;
        } else if (it->isRevoked()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "a userid is revoked";
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "bad validity" << int(it->validity());
        }
    }
    return false;
}

static bool ValidSMIMEEncryptionKey(const GpgME::Key &key)
{
    if (key.protocol() != GpgME::CMS) {
        return false;
    }
    if (key.isRevoked() || key.isExpired() || key.isDisabled() || !key.canEncrypt()) {
        return false;
    }
    return true;
}

static bool ValidTrustedSMIMEEncryptionKey(const GpgME::Key &key)
{
    if (!ValidSMIMEEncryptionKey(key)) {
        return false;
    }
    return true;
}

static inline bool ValidTrustedEncryptionKey(const GpgME::Key &key)
{
    switch (key.protocol()) {
    case GpgME::OpenPGP:
        return ValidTrustedOpenPGPEncryptionKey(key);
    case GpgME::CMS:
        return ValidTrustedSMIMEEncryptionKey(key);
    default:
        return false;
    }
}

static inline bool ValidEncryptionKey(const GpgME::Key &key)
{
    switch (key.protocol()) {
    case GpgME::OpenPGP:
        return ValidOpenPGPEncryptionKey(key);
    case GpgME::CMS:
        return ValidSMIMEEncryptionKey(key);
    default:
        return false;
    }
}

static inline bool ValidSigningKey(const GpgME::Key &key)
{
    if (key.isRevoked() || key.isExpired() || key.isDisabled() || !key.canSign()) {
        return false;
    }
    return key.hasSecret();
}

static inline bool ValidOpenPGPSigningKey(const GpgME::Key &key)
{
    return key.protocol() == GpgME::OpenPGP && ValidSigningKey(key);
}

static inline bool ValidSMIMESigningKey(const GpgME::Key &key)
{
    return key.protocol() == GpgME::CMS && ValidSigningKey(key);
}

static inline bool NotValidTrustedOpenPGPEncryptionKey(const GpgME::Key &key)
{
    return !ValidTrustedOpenPGPEncryptionKey(key);
}

static inline bool NotValidTrustedSMIMEEncryptionKey(const GpgME::Key &key)
{
    return !ValidTrustedSMIMEEncryptionKey(key);
}

static inline bool NotValidTrustedEncryptionKey(const GpgME::Key &key)
{
    return !ValidTrustedEncryptionKey(key);
}

static inline bool NotValidEncryptionKey(const GpgME::Key &key)
{
    return !ValidEncryptionKey(key);
}

static inline bool NotValidOpenPGPSigningKey(const GpgME::Key &key)
{
    return !ValidOpenPGPSigningKey(key);
}

static inline bool NotValidSMIMESigningKey(const GpgME::Key &key)
{
    return !ValidSMIMESigningKey(key);
}

namespace
{
struct ByTrustScore {
    static int score(const GpgME::UserID &uid)
    {
        return uid.isRevoked() || uid.isInvalid() ? -1 : uid.validity();
    }

    bool operator()(const GpgME::UserID &lhs, const GpgME::UserID &rhs) const
    {
        return score(lhs) < score(rhs);
    }
};
}

static std::vector<GpgME::UserID> matchingUIDs(const std::vector<GpgME::UserID> &uids, const QString &address)
{
    if (address.isEmpty()) {
        return {};
    }

    std::vector<GpgME::UserID> result;
    result.reserve(uids.size());

    for (auto it = uids.begin(), end = uids.end(); it != end; ++it) {
        // PENDING(marc) check DN for an EMAIL, too, in case of X.509 certs... :/
        if (const char *email = it->email()) {
            if (*email && QString::fromUtf8(email).simplified().toLower() == address) {
                result.push_back(*it);
            }
        }
    }
    return result;
}

static GpgME::UserID findBestMatchUID(const GpgME::Key &key, const QString &address)
{
    const std::vector<GpgME::UserID> all = key.userIDs();
    if (all.empty()) {
        return {};
    }
    const std::vector<GpgME::UserID> matching = matchingUIDs(all, address.toLower());
    const std::vector<GpgME::UserID> &v = matching.empty() ? all : matching;
    return *std::max_element(v.begin(), v.end(), ByTrustScore());
}

static QStringList keysAsStrings(const std::vector<GpgME::Key> &keys)
{
    QStringList strings;
    strings.reserve(keys.size());
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        assert(!(*it).userID(0).isNull());
        const auto userID = (*it).userID(0);
        QString keyLabel = QString::fromUtf8(userID.email());
        if (keyLabel.isEmpty()) {
            keyLabel = QString::fromUtf8(userID.name());
        }
        if (keyLabel.isEmpty()) {
            keyLabel = QString::fromUtf8(userID.id());
        }
        strings.append(keyLabel);
    }
    return strings;
}

static std::vector<GpgME::Key> trustedOrConfirmed(const std::vector<GpgME::Key> &keys, const QString &address, bool &canceled)
{
    // PENDING(marc) work on UserIDs here?
    std::vector<GpgME::Key> fishies;
    std::vector<GpgME::Key> ickies;
    std::vector<GpgME::Key> rewookies;
    auto it = keys.begin();
    const auto end = keys.end();
    for (; it != end; ++it) {
        const GpgME::Key &key = *it;
        assert(ValidEncryptionKey(key));
        const GpgME::UserID uid = findBestMatchUID(key, address);
        if (uid.isRevoked()) {
            rewookies.push_back(key);
        }
        if (!uid.isRevoked()) {
            if (uid.validity() == GpgME::UserID::Marginal) {
                fishies.push_back(key);
            }
            if (uid.validity() < GpgME::UserID::Never) {
                ickies.push_back(key);
            }
        }
    }

    if (fishies.empty() && ickies.empty() && rewookies.empty()) {
        return keys;
    }

    // if  some keys are not fully trusted, let the user confirm their use
    QString msg = address.isEmpty() ? i18n(
                      "One or more of your configured OpenPGP encryption "
                      "keys or S/MIME certificates is not fully trusted "
                      "for encryption.")
                                    : i18n(
                                        "One or more of the OpenPGP encryption keys or S/MIME "
                                        "certificates for recipient \"%1\" is not fully trusted "
                                        "for encryption.",
                                        address);

    if (!fishies.empty()) {
        // certificates can't have marginal trust
        msg += i18n("\nThe following keys are only marginally trusted: \n");
        msg += keysAsStrings(fishies).join(QLatin1Char(','));
    }
    if (!ickies.empty()) {
        msg += i18n("\nThe following keys or certificates have unknown trust level: \n");
        msg += keysAsStrings(ickies).join(QLatin1Char(','));
    }
    if (!rewookies.empty()) {
        msg += i18n("\nThe following keys or certificates are <b>revoked</b>: \n");
        msg += keysAsStrings(rewookies).join(QLatin1Char(','));
    }

    if (KMessageBox::warningContinueCancel(nullptr,
                                           msg,
                                           i18nc("@title:window", "Not Fully Trusted Encryption Keys"),
                                           KStandardGuiItem::cont(),
                                           KStandardGuiItem::cancel(),
                                           QStringLiteral("not fully trusted encryption key warning"))
        == KMessageBox::Continue) {
        return keys;
    } else {
        canceled = true;
    }
    return {};
}

namespace
{
struct IsNotForFormat : public std::function<bool(GpgME::Key)> {
    IsNotForFormat(Kleo::CryptoMessageFormat f)
        : format(f)
    {
    }

    bool operator()(const GpgME::Key &key) const
    {
        return (isOpenPGP(format) && key.protocol() != GpgME::OpenPGP) || (isSMIME(format) && key.protocol() != GpgME::CMS);
    }

    const Kleo::CryptoMessageFormat format;
};

struct IsForFormat : std::function<bool(GpgME::Key)> {
    explicit IsForFormat(Kleo::CryptoMessageFormat f)
        : protocol(isOpenPGP(f)     ? GpgME::OpenPGP
                       : isSMIME(f) ? GpgME::CMS
                                    : GpgME::UnknownProtocol)
    {
    }

    bool operator()(const GpgME::Key &key) const
    {
        return key.protocol() == protocol;
    }

    const GpgME::Protocol protocol;
};
}

class Kleo::KeyResolver::SigningPreferenceCounter : public std::function<void(Kleo::KeyResolver::Item)>
{
public:
    SigningPreferenceCounter() = default;

    void operator()(const Kleo::KeyResolver::Item &item);
#define make_int_accessor(x)                                                                                                                                   \
    unsigned int num##x() const                                                                                                                                \
    {                                                                                                                                                          \
        return m##x;                                                                                                                                           \
    }
    make_int_accessor(UnknownSigningPreference) make_int_accessor(NeverSign) make_int_accessor(AlwaysSign) make_int_accessor(AlwaysSignIfPossible)
        make_int_accessor(AlwaysAskForSigning) make_int_accessor(AskSigningWheneverPossible) make_int_accessor(Total)
#undef make_int_accessor
            private : unsigned int mTotal = 0;
    unsigned int mUnknownSigningPreference = 0;
    unsigned int mNeverSign = 0;
    unsigned int mAlwaysSign = 0;
    unsigned int mAlwaysSignIfPossible = 0;
    unsigned int mAlwaysAskForSigning = 0;
    unsigned int mAskSigningWheneverPossible = 0;
};

void Kleo::KeyResolver::SigningPreferenceCounter::operator()(const Kleo::KeyResolver::Item &item)
{
    switch (item.signPref) {
#define CASE(x)                                                                                                                                                \
    case x:                                                                                                                                                    \
        ++m##x;                                                                                                                                                \
        break
        CASE(UnknownSigningPreference);
        CASE(NeverSign);
        CASE(AlwaysSign);
        CASE(AlwaysSignIfPossible);
        CASE(AlwaysAskForSigning);
        CASE(AskSigningWheneverPossible);
#undef CASE
    }
    ++mTotal;
}

class Kleo::KeyResolver::EncryptionPreferenceCounter : public std::function<void(Item)>
{
    const Kleo::KeyResolver *_this;

public:
    EncryptionPreferenceCounter(const Kleo::KeyResolver *kr, EncryptionPreference defaultPreference)
        : _this(kr)
        , mDefaultPreference(defaultPreference)
    {
    }

    void operator()(Item &item);

    template<typename Container>
    void process(Container &c)
    {
        *this = std::for_each(c.begin(), c.end(), *this);
    }

#define make_int_accessor(x)                                                                                                                                   \
    unsigned int num##x() const                                                                                                                                \
    {                                                                                                                                                          \
        return m##x;                                                                                                                                           \
    }
    make_int_accessor(NoKey) make_int_accessor(NeverEncrypt) make_int_accessor(UnknownPreference) make_int_accessor(AlwaysEncrypt)
        make_int_accessor(AlwaysEncryptIfPossible) make_int_accessor(AlwaysAskForEncryption) make_int_accessor(AskWheneverPossible) make_int_accessor(Total)
#undef make_int_accessor
            private : EncryptionPreference mDefaultPreference;
    unsigned int mTotal = 0;
    unsigned int mNoKey = 0;
    unsigned int mNeverEncrypt = 0;
    unsigned int mUnknownPreference = 0;
    unsigned int mAlwaysEncrypt = 0;
    unsigned int mAlwaysEncryptIfPossible = 0;
    unsigned int mAlwaysAskForEncryption = 0;
    unsigned int mAskWheneverPossible = 0;
};

void Kleo::KeyResolver::EncryptionPreferenceCounter::operator()(Item &item)
{
    if (_this) {
        if (item.needKeys) {
            item.keys = _this->getEncryptionKeys(item.address, true);
        }
        if (item.keys.empty()) {
            ++mNoKey;
            return;
        }
    }
    switch (!item.pref ? mDefaultPreference : item.pref) {
#define CASE(x)                                                                                                                                                \
    case Kleo::x:                                                                                                                                              \
        ++m##x;                                                                                                                                                \
        break
        CASE(NeverEncrypt);
        CASE(UnknownPreference);
        CASE(AlwaysEncrypt);
        CASE(AlwaysEncryptIfPossible);
        CASE(AlwaysAskForEncryption);
        CASE(AskWheneverPossible);
#undef CASE
    }
    ++mTotal;
}

namespace
{
class FormatPreferenceCounterBase : public std::function<void(Kleo::KeyResolver::Item)>
{
public:
    FormatPreferenceCounterBase() = default;

#define make_int_accessor(x)                                                                                                                                   \
    unsigned int num##x() const                                                                                                                                \
    {                                                                                                                                                          \
        return m##x;                                                                                                                                           \
    }
    make_int_accessor(Total) make_int_accessor(InlineOpenPGP) make_int_accessor(OpenPGPMIME) make_int_accessor(SMIME) make_int_accessor(SMIMEOpaque)
#undef make_int_accessor

        [[nodiscard]] unsigned int numOf(Kleo::CryptoMessageFormat f) const
    {
        switch (f) {
#define CASE(x)                                                                                                                                                \
    case Kleo::x##Format:                                                                                                                                      \
        return m##x
            CASE(InlineOpenPGP);
            CASE(OpenPGPMIME);
            CASE(SMIME);
            CASE(SMIMEOpaque);
#undef CASE
        default:
            return 0;
        }
    }

protected:
    unsigned int mTotal = 0;
    unsigned int mInlineOpenPGP = 0;
    unsigned int mOpenPGPMIME = 0;
    unsigned int mSMIME = 0;
    unsigned int mSMIMEOpaque = 0;
};

class EncryptionFormatPreferenceCounter : public FormatPreferenceCounterBase
{
public:
    EncryptionFormatPreferenceCounter()
        : FormatPreferenceCounterBase()
    {
    }

    void operator()(const Kleo::KeyResolver::Item &item);
};

class SigningFormatPreferenceCounter : public FormatPreferenceCounterBase
{
public:
    SigningFormatPreferenceCounter()
        : FormatPreferenceCounterBase()
    {
    }

    void operator()(const Kleo::KeyResolver::Item &item);
};

#define CASE(x)                                                                                                                                                \
    if (item.format & Kleo::x##Format) {                                                                                                                       \
        ++m##x;                                                                                                                                                \
    }
void EncryptionFormatPreferenceCounter::operator()(const Kleo::KeyResolver::Item &item)
{
    if (item.format & (Kleo::InlineOpenPGPFormat | Kleo::OpenPGPMIMEFormat)
        && std::any_of(item.keys.begin(),
                       item.keys.end(),
                       ValidTrustedOpenPGPEncryptionKey)) { // -= trusted?
        CASE(OpenPGPMIME);
        CASE(InlineOpenPGP);
    }
    if (item.format & (Kleo::SMIMEFormat | Kleo::SMIMEOpaqueFormat)
        && std::any_of(item.keys.begin(),
                       item.keys.end(),
                       ValidTrustedSMIMEEncryptionKey)) { // -= trusted?
        CASE(SMIME);
        CASE(SMIMEOpaque);
    }
    ++mTotal;
}

void SigningFormatPreferenceCounter::operator()(const Kleo::KeyResolver::Item &item)
{
    CASE(InlineOpenPGP);
    CASE(OpenPGPMIME);
    CASE(SMIME);
    CASE(SMIMEOpaque);
    ++mTotal;
}

#undef CASE
} // anon namespace

static QString canonicalAddress(const QString &_address)
{
    const QString address = KEmailAddress::extractEmailAddress(_address);
    if (!address.contains(QLatin1Char('@'))) {
        // local address
        // return address + '@' + KNetwork::KResolver::localHostName();
        return address + QLatin1StringView("@localdomain");
    } else {
        return address;
    }
}

struct FormatInfo {
    std::vector<Kleo::KeyResolver::SplitInfo> splitInfos;
    std::vector<GpgME::Key> signKeys;
};

struct Q_DECL_HIDDEN Kleo::KeyResolver::KeyResolverPrivate {
    bool mAkonadiLookupEnabled = true;
    bool mAutocryptEnabled = false;
    std::set<QByteArray> alreadyWarnedFingerprints;

    std::vector<GpgME::Key> mOpenPGPSigningKeys; // signing
    std::vector<GpgME::Key> mSMIMESigningKeys; // signing

    std::vector<GpgME::Key> mOpenPGPEncryptToSelfKeys; // encryption to self
    std::vector<GpgME::Key> mSMIMEEncryptToSelfKeys; // encryption to self

    std::vector<Item> mPrimaryEncryptionKeys; // encryption to To/CC
    std::vector<Item> mSecondaryEncryptionKeys; // encryption to BCC

    std::map<CryptoMessageFormat, FormatInfo> mFormatInfoMap;

    // key=email address, value=crypto preferences for this contact (from kabc)
    using ContactPreferencesMap = std::map<QString, MessageComposer::ContactPreference>;
    ContactPreferencesMap mContactPreferencesMap;
    std::map<QByteArray, QString> mAutocryptMap;
    std::shared_ptr<Kleo::ExpiryChecker> expiryChecker;
};

Kleo::KeyResolver::KeyResolver(bool encToSelf, bool showApproval, bool oppEncryption, unsigned int f, const std::shared_ptr<Kleo::ExpiryChecker> &expiryChecker)
    : d(new KeyResolverPrivate)
    , mEncryptToSelf(encToSelf)
    , mShowApprovalDialog(showApproval)
    , mOpportunisticEncyption(oppEncryption)
    , mCryptoMessageFormats(f)
{
    d->expiryChecker = expiryChecker;
}

Kleo::KeyResolver::~KeyResolver() = default;

Kleo::Result Kleo::KeyResolver::setEncryptToSelfKeys(const QStringList &fingerprints)
{
    if (!encryptToSelf()) {
        return Kleo::Ok;
    }

    std::vector<GpgME::Key> keys = lookup(fingerprints);
    std::remove_copy_if(keys.begin(), keys.end(), std::back_inserter(d->mOpenPGPEncryptToSelfKeys),
                        NotValidTrustedOpenPGPEncryptionKey); // -= trusted?
    std::remove_copy_if(keys.begin(), keys.end(), std::back_inserter(d->mSMIMEEncryptToSelfKeys),
                        NotValidTrustedSMIMEEncryptionKey); // -= trusted?

    if (d->mOpenPGPEncryptToSelfKeys.size() + d->mSMIMEEncryptToSelfKeys.size() < keys.size()) {
        // too few keys remain...
        const QString msg = i18n(
            "One or more of your configured OpenPGP encryption "
            "keys or S/MIME certificates is not usable for "
            "encryption. Please reconfigure your encryption keys "
            "and certificates for this identity in the identity "
            "configuration dialog.\n"
            "If you choose to continue, and the keys are needed "
            "later on, you will be prompted to specify the keys "
            "to use.");
        return KMessageBox::warningContinueCancel(nullptr,
                                                  msg,
                                                  i18nc("@title:window", "Unusable Encryption Keys"),
                                                  KStandardGuiItem::cont(),
                                                  KStandardGuiItem::cancel(),
                                                  QStringLiteral("unusable own encryption key warning"))
                == KMessageBox::Continue
            ? Kleo::Ok
            : Kleo::Canceled;
    }

    // check for near-expiry:
    std::vector<GpgME::Key>::const_iterator end(d->mOpenPGPEncryptToSelfKeys.end());

    for (auto it = d->mOpenPGPEncryptToSelfKeys.begin(); it != end; ++it) {
        d->expiryChecker->checkKey(*it, Kleo::ExpiryChecker::OwnEncryptionKey);
    }
    std::vector<GpgME::Key>::const_iterator end2(d->mSMIMEEncryptToSelfKeys.end());
    for (auto it = d->mSMIMEEncryptToSelfKeys.begin(); it != end2; ++it) {
        d->expiryChecker->checkKey(*it, Kleo::ExpiryChecker::OwnEncryptionKey);
    }

    return Kleo::Ok;
}

Kleo::Result Kleo::KeyResolver::setSigningKeys(const QStringList &fingerprints)
{
    std::vector<GpgME::Key> keys = lookup(fingerprints, true); // secret keys
    std::remove_copy_if(keys.begin(), keys.end(), std::back_inserter(d->mOpenPGPSigningKeys), NotValidOpenPGPSigningKey);
    std::remove_copy_if(keys.begin(), keys.end(), std::back_inserter(d->mSMIMESigningKeys), NotValidSMIMESigningKey);

    if (d->mOpenPGPSigningKeys.size() + d->mSMIMESigningKeys.size() < keys.size()) {
        // too few keys remain...
        const QString msg = i18n(
            "One or more of your configured OpenPGP signing keys "
            "or S/MIME signing certificates is not usable for "
            "signing. Please reconfigure your signing keys "
            "and certificates for this identity in the identity "
            "configuration dialog.\n"
            "If you choose to continue, and the keys are needed "
            "later on, you will be prompted to specify the keys "
            "to use.");
        return KMessageBox::warningContinueCancel(nullptr,
                                                  msg,
                                                  i18nc("@title:window", "Unusable Signing Keys"),
                                                  KStandardGuiItem::cont(),
                                                  KStandardGuiItem::cancel(),
                                                  QStringLiteral("unusable signing key warning"))
                == KMessageBox::Continue
            ? Kleo::Ok
            : Kleo::Canceled;
    }

    // check for near expiry:

    for (auto it = d->mOpenPGPSigningKeys.begin(), total = d->mOpenPGPSigningKeys.end(); it != total; ++it) {
        d->expiryChecker->checkKey(*it, Kleo::ExpiryChecker::OwnSigningKey);
    }

    for (auto it = d->mSMIMESigningKeys.begin(), total = d->mSMIMESigningKeys.end(); it != total; ++it) {
        d->expiryChecker->checkKey(*it, Kleo::ExpiryChecker::OwnSigningKey);
    }

    return Kleo::Ok;
}

void Kleo::KeyResolver::setPrimaryRecipients(const QStringList &addresses)
{
    d->mPrimaryEncryptionKeys = getEncryptionItems(addresses);
}

void Kleo::KeyResolver::setSecondaryRecipients(const QStringList &addresses)
{
    d->mSecondaryEncryptionKeys = getEncryptionItems(addresses);
}

std::vector<Kleo::KeyResolver::Item> Kleo::KeyResolver::getEncryptionItems(const QStringList &addresses)
{
    std::vector<Item> items;
    items.reserve(addresses.size());
    QStringList::const_iterator end(addresses.constEnd());
    for (QStringList::const_iterator it = addresses.constBegin(); it != end; ++it) {
        QString addr = canonicalAddress(*it).toLower();
        const auto pref = lookupContactPreferences(addr);

        items.emplace_back(*it, /*getEncryptionKeys( *it, true ),*/
                           pref.encryptionPreference,
                           pref.signingPreference,
                           pref.cryptoMessageFormat);
    }
    return items;
}

static Kleo::Action action(bool doit, bool ask, bool donot, bool requested)
{
    if (requested && !donot) {
        return Kleo::DoIt;
    }
    if (doit && !ask && !donot) {
        return Kleo::DoIt;
    }
    if (!doit && ask && !donot) {
        return Kleo::Ask;
    }
    if (!doit && !ask && donot) {
        return requested ? Kleo::Conflict : Kleo::DontDoIt;
    }
    if (!doit && !ask && !donot) {
        return Kleo::DontDoIt;
    }
    return Kleo::Conflict;
}

Kleo::Action Kleo::KeyResolver::checkSigningPreferences(bool signingRequested) const
{
    if (signingRequested && d->mOpenPGPSigningKeys.empty() && d->mSMIMESigningKeys.empty()) {
        return Impossible;
    }

    SigningPreferenceCounter count;
    count = std::for_each(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), count);
    count = std::for_each(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), count);

    unsigned int sign = count.numAlwaysSign();
    unsigned int ask = count.numAlwaysAskForSigning();
    const unsigned int dontSign = count.numNeverSign();
    if (signingPossible()) {
        sign += count.numAlwaysSignIfPossible();
        ask += count.numAskSigningWheneverPossible();
    }

    return action(sign, ask, dontSign, signingRequested);
}

bool Kleo::KeyResolver::signingPossible() const
{
    return !d->mOpenPGPSigningKeys.empty() || !d->mSMIMESigningKeys.empty();
}

Kleo::Action Kleo::KeyResolver::checkEncryptionPreferences(bool encryptionRequested) const
{
    if (d->mPrimaryEncryptionKeys.empty() && d->mSecondaryEncryptionKeys.empty()) {
        return DontDoIt;
    }

    if (encryptionRequested && encryptToSelf() && d->mOpenPGPEncryptToSelfKeys.empty() && d->mSMIMEEncryptToSelfKeys.empty()) {
        return Impossible;
    }

    if (!encryptionRequested && !mOpportunisticEncyption) {
        // try to minimize crypto ops (including key lookups) by only
        // looking up keys when at least one of the encryption
        // preferences needs it:
        EncryptionPreferenceCounter count(nullptr, UnknownPreference);
        count.process(d->mPrimaryEncryptionKeys);
        count.process(d->mSecondaryEncryptionKeys);
        if (!count.numAlwaysEncrypt()
            && !count.numAlwaysAskForEncryption() // this guy might not need a lookup, when declined, but it's too complex to implement that here
            && !count.numAlwaysEncryptIfPossible() && !count.numAskWheneverPossible()) {
            return DontDoIt;
        }
    }

    EncryptionPreferenceCounter count(this, mOpportunisticEncyption ? AskWheneverPossible : UnknownPreference);
    count = std::for_each(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), count);
    count = std::for_each(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), count);

    unsigned int encrypt = count.numAlwaysEncrypt();
    unsigned int ask = count.numAlwaysAskForEncryption();
    const unsigned int dontEncrypt = count.numNeverEncrypt() + count.numNoKey();
    if (encryptionPossible()) {
        encrypt += count.numAlwaysEncryptIfPossible();
        ask += count.numAskWheneverPossible();
    }

    const Action act = action(encrypt, ask, dontEncrypt, encryptionRequested);
    if (act != Ask
        || std::for_each(
               d->mPrimaryEncryptionKeys.begin(),
               d->mPrimaryEncryptionKeys.end(),
               std::for_each(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), EncryptionPreferenceCounter(this, UnknownPreference)))
               .numAlwaysAskForEncryption()) {
        return act;
    } else {
        return AskOpportunistic;
    }
}

bool Kleo::KeyResolver::encryptionPossible() const
{
    return std::none_of(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), EmptyKeyList)
        && std::none_of(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), EmptyKeyList);
}

Kleo::Result Kleo::KeyResolver::resolveAllKeys(bool &signingRequested, bool &encryptionRequested)
{
    if (!encryptionRequested && !signingRequested) {
        // make a dummy entry with all recipients, but no signing or
        // encryption keys to avoid special-casing on the caller side:
        dump();
        d->mFormatInfoMap[OpenPGPMIMEFormat].splitInfos.emplace_back(allRecipients());
        dump();
        return Kleo::Ok;
    }
    Kleo::Result result = Kleo::Ok;
    if (encryptionRequested) {
        bool finalySendUnencrypted = false;
        result = resolveEncryptionKeys(signingRequested, finalySendUnencrypted);
        if (finalySendUnencrypted) {
            encryptionRequested = false;
        }
    }
    if (result != Kleo::Ok) {
        return result;
    }
    if (encryptionRequested) {
        result = resolveSigningKeysForEncryption();
    } else {
        result = resolveSigningKeysForSigningOnly();
        if (result == Kleo::Failure) {
            signingRequested = false;
            return Kleo::Ok;
        }
    }
    return result;
}

Kleo::Result Kleo::KeyResolver::resolveEncryptionKeys(bool signingRequested, bool &finalySendUnencrypted)
{
    //
    // 1. Get keys for all recipients:
    //
    qCDebug(MESSAGECOMPOSER_LOG) << "resolving enc keys" << d->mPrimaryEncryptionKeys.size();
    for (auto it = d->mPrimaryEncryptionKeys.begin(); it != d->mPrimaryEncryptionKeys.end(); ++it) {
        qCDebug(MESSAGECOMPOSER_LOG) << "checking primary:" << it->address;
        if (!it->needKeys) {
            continue;
        }
        it->keys = getEncryptionKeys(it->address, false);
        qCDebug(MESSAGECOMPOSER_LOG) << "got # keys:" << it->keys.size();
        if (it->keys.empty()) {
            return Kleo::Canceled;
        }
        QString addr = canonicalAddress(it->address).toLower();
        const auto pref = lookupContactPreferences(addr);
        it->pref = pref.encryptionPreference;
        it->signPref = pref.signingPreference;
        it->format = pref.cryptoMessageFormat;
        qCDebug(MESSAGECOMPOSER_LOG) << "set key data:" << int(it->pref) << int(it->signPref) << int(it->format);
    }

    for (auto it = d->mSecondaryEncryptionKeys.begin(), total = d->mSecondaryEncryptionKeys.end(); it != total; ++it) {
        if (!it->needKeys) {
            continue;
        }
        it->keys = getEncryptionKeys(it->address, false);
        if (it->keys.empty()) {
            return Kleo::Canceled;
        }
        QString addr = canonicalAddress(it->address).toLower();
        const auto pref = lookupContactPreferences(addr);
        it->pref = pref.encryptionPreference;
        it->signPref = pref.signingPreference;
        it->format = pref.cryptoMessageFormat;
    }

    // 1a: Present them to the user

    const Kleo::Result res = showKeyApprovalDialog(finalySendUnencrypted);
    if (res != Kleo::Ok) {
        return res;
    }

    //
    // 2. Check what the primary recipients need
    //

    // 2a. Try to find a common format for all primary recipients,
    //     else use as many formats as needed

    const EncryptionFormatPreferenceCounter primaryCount =
        std::for_each(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), EncryptionFormatPreferenceCounter());

    CryptoMessageFormat commonFormat = AutoFormat;
    for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
        if (!(concreteCryptoMessageFormats[i] & mCryptoMessageFormats)) {
            continue;
        }
        if (signingRequested && signingKeysFor(concreteCryptoMessageFormats[i]).empty()) {
            continue;
        }
        if (encryptToSelf() && encryptToSelfKeysFor(concreteCryptoMessageFormats[i]).empty()) {
            continue;
        }
        if (primaryCount.numOf(concreteCryptoMessageFormats[i]) == primaryCount.numTotal()) {
            commonFormat = concreteCryptoMessageFormats[i];
            break;
        }
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "got commonFormat for primary recipients:" << int(commonFormat);
    if (commonFormat != AutoFormat) {
        addKeys(d->mPrimaryEncryptionKeys, commonFormat);
    } else {
        addKeys(d->mPrimaryEncryptionKeys);
    }

    collapseAllSplitInfos(); // these can be encrypted together

    // 2b. Just try to find _something_ for each secondary recipient,
    //     with a preference to a common format (if that exists)

    const EncryptionFormatPreferenceCounter secondaryCount =
        std::for_each(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), EncryptionFormatPreferenceCounter());

    if (commonFormat != AutoFormat && secondaryCount.numOf(commonFormat) == secondaryCount.numTotal()) {
        addKeys(d->mSecondaryEncryptionKeys, commonFormat);
    } else {
        addKeys(d->mSecondaryEncryptionKeys);
    }

    // 3. Check for expiry:

    for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
        const std::vector<SplitInfo> si_list = encryptionItems(concreteCryptoMessageFormats[i]);
        for (auto sit = si_list.begin(), total = si_list.end(); sit != total; ++sit) {
            for (auto kit = sit->keys.begin(); kit != sit->keys.end(); ++kit) {
                d->expiryChecker->checkKey(*kit, Kleo::ExpiryChecker::EncryptionKey);
            }
        }
    }

    // 4. Check that we have the right keys for encryptToSelf()

    if (!encryptToSelf()) {
        return Kleo::Ok;
    }

    // 4a. Check for OpenPGP keys

    qCDebug(MESSAGECOMPOSER_LOG) << "sizes of encryption items:" << encryptionItems(InlineOpenPGPFormat).size() << encryptionItems(OpenPGPMIMEFormat).size()
                                 << encryptionItems(SMIMEFormat).size() << encryptionItems(SMIMEOpaqueFormat).size();
    if (!encryptionItems(InlineOpenPGPFormat).empty() || !encryptionItems(OpenPGPMIMEFormat).empty()) {
        // need them
        if (d->mOpenPGPEncryptToSelfKeys.empty()) {
            const QString msg = i18n(
                "Examination of recipient's encryption preferences "
                "yielded that the message should be encrypted using "
                "OpenPGP, at least for some recipients;\n"
                "however, you have not configured valid trusted "
                "OpenPGP encryption keys for this identity.\n"
                "You may continue without encrypting to yourself, "
                "but be aware that you will not be able to read your "
                "own messages if you do so.");
            if (KMessageBox::warningContinueCancel(nullptr,
                                                   msg,
                                                   i18nc("@title:window", "Unusable Encryption Keys"),
                                                   KStandardGuiItem::cont(),
                                                   KStandardGuiItem::cancel(),
                                                   QStringLiteral("encrypt-to-self will fail warning"))
                == KMessageBox::Cancel) {
                return Kleo::Canceled;
            }
            // FIXME: Allow selection
        }
        addToAllSplitInfos(d->mOpenPGPEncryptToSelfKeys, InlineOpenPGPFormat | OpenPGPMIMEFormat);
    }

    // 4b. Check for S/MIME certs:

    if (!encryptionItems(SMIMEFormat).empty() || !encryptionItems(SMIMEOpaqueFormat).empty()) {
        // need them
        if (d->mSMIMEEncryptToSelfKeys.empty()) {
            // don't have one
            const QString msg = i18n(
                "Examination of recipient's encryption preferences "
                "yielded that the message should be encrypted using "
                "S/MIME, at least for some recipients;\n"
                "however, you have not configured valid "
                "S/MIME encryption certificates for this identity.\n"
                "You may continue without encrypting to yourself, "
                "but be aware that you will not be able to read your "
                "own messages if you do so.");
            if (KMessageBox::warningContinueCancel(nullptr,
                                                   msg,
                                                   i18nc("@title:window", "Unusable Encryption Keys"),
                                                   KStandardGuiItem::cont(),
                                                   KStandardGuiItem::cancel(),
                                                   QStringLiteral("encrypt-to-self will fail warning"))
                == KMessageBox::Cancel) {
                return Kleo::Canceled;
            }
            // FIXME: Allow selection
        }
        addToAllSplitInfos(d->mSMIMEEncryptToSelfKeys, SMIMEFormat | SMIMEOpaqueFormat);
    }

    // FIXME: Present another message if _both_ OpenPGP and S/MIME keys
    // are missing.

    return Kleo::Ok;
}

Kleo::Result Kleo::KeyResolver::resolveSigningKeysForEncryption()
{
    if ((!encryptionItems(InlineOpenPGPFormat).empty() || !encryptionItems(OpenPGPMIMEFormat).empty()) && d->mOpenPGPSigningKeys.empty()) {
        const QString msg = i18n(
            "Examination of recipient's signing preferences "
            "yielded that the message should be signed using "
            "OpenPGP, at least for some recipients;\n"
            "however, you have not configured valid "
            "OpenPGP signing certificates for this identity.");
        if (KMessageBox::warningContinueCancel(nullptr,
                                               msg,
                                               i18nc("@title:window", "Unusable Signing Keys"),
                                               KGuiItem(i18nc("@action:button", "Do Not OpenPGP-Sign")),
                                               KStandardGuiItem::cancel(),
                                               QStringLiteral("signing will fail warning"))
            == KMessageBox::Cancel) {
            return Kleo::Canceled;
        }
        // FIXME: Allow selection
    }
    if ((!encryptionItems(SMIMEFormat).empty() || !encryptionItems(SMIMEOpaqueFormat).empty()) && d->mSMIMESigningKeys.empty()) {
        const QString msg = i18n(
            "Examination of recipient's signing preferences "
            "yielded that the message should be signed using "
            "S/MIME, at least for some recipients;\n"
            "however, you have not configured valid "
            "S/MIME signing certificates for this identity.");
        if (KMessageBox::warningContinueCancel(nullptr,
                                               msg,
                                               i18nc("@title:window", "Unusable Signing Keys"),
                                               KGuiItem(i18nc("@action:button", "Do Not S/MIME-Sign")),
                                               KStandardGuiItem::cancel(),
                                               QStringLiteral("signing will fail warning"))
            == KMessageBox::Cancel) {
            return Kleo::Canceled;
        }
        // FIXME: Allow selection
    }

    // FIXME: Present another message if _both_ OpenPGP and S/MIME keys
    // are missing.

    for (auto it = d->mFormatInfoMap.begin(); it != d->mFormatInfoMap.end(); ++it) {
        if (!it->second.splitInfos.empty()) {
            dump();
            it->second.signKeys = signingKeysFor(it->first);
            dump();
        }
    }

    return Kleo::Ok;
}

Kleo::Result Kleo::KeyResolver::resolveSigningKeysForSigningOnly()
{
    //
    // we don't need to distinguish between primary and secondary
    // recipients here:
    //
    SigningFormatPreferenceCounter count;
    count = std::for_each(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), count);
    count = std::for_each(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), count);

    // try to find a common format that works for all (and that we have signing keys for):

    CryptoMessageFormat commonFormat = AutoFormat;

    for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
        const auto res = concreteCryptoMessageFormats[i];
        if (!(mCryptoMessageFormats & res)) {
            continue; // skip
        }
        if (signingKeysFor(res).empty()) {
            continue; // skip
        }
        if (count.numOf(res) == count.numTotal()) {
            commonFormat = res;
            break;
        }
    }

    if (commonFormat != AutoFormat) { // found
        dump();
        FormatInfo &fi = d->mFormatInfoMap[commonFormat];
        fi.signKeys = signingKeysFor(commonFormat);
        fi.splitInfos.resize(1);
        fi.splitInfos.front() = SplitInfo(allRecipients());
        dump();
        return Kleo::Ok;
    }

    const QString msg = i18n(
        "Examination of recipient's signing preferences "
        "showed no common type of signature matching your "
        "available signing keys.\n"
        "Send message without signing?");
    if (KMessageBox::warningContinueCancel(nullptr, msg, i18nc("@title:window", "No signing possible"), KStandardGuiItem::cont()) == KMessageBox::Continue) {
        d->mFormatInfoMap[OpenPGPMIMEFormat].splitInfos.emplace_back(allRecipients());
        return Kleo::Failure; // means "Ok, but without signing"
    }
    return Kleo::Canceled;
}

std::vector<GpgME::Key> Kleo::KeyResolver::signingKeysFor(CryptoMessageFormat f) const
{
    if (isOpenPGP(f)) {
        return d->mOpenPGPSigningKeys;
    }
    if (isSMIME(f)) {
        return d->mSMIMESigningKeys;
    }
    return {};
}

std::vector<GpgME::Key> Kleo::KeyResolver::encryptToSelfKeysFor(CryptoMessageFormat f) const
{
    if (isOpenPGP(f)) {
        return d->mOpenPGPEncryptToSelfKeys;
    }
    if (isSMIME(f)) {
        return d->mSMIMEEncryptToSelfKeys;
    }
    return {};
}

QStringList Kleo::KeyResolver::allRecipients() const
{
    QStringList result;
    std::transform(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), std::back_inserter(result), ItemDotAddress);
    std::transform(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), std::back_inserter(result), ItemDotAddress);
    return result;
}

void Kleo::KeyResolver::collapseAllSplitInfos()
{
    dump();
    for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
        auto pos = d->mFormatInfoMap.find(concreteCryptoMessageFormats[i]);
        if (pos == d->mFormatInfoMap.end()) {
            continue;
        }
        std::vector<SplitInfo> &v = pos->second.splitInfos;
        if (v.size() < 2) {
            continue;
        }
        SplitInfo &si = v.front();
        for (auto it = v.begin() + 1; it != v.end(); ++it) {
            si.keys.insert(si.keys.end(), it->keys.begin(), it->keys.end());
            std::copy(it->recipients.begin(), it->recipients.end(), std::back_inserter(si.recipients));
        }
        v.resize(1);
    }
    dump();
}

void Kleo::KeyResolver::addToAllSplitInfos(const std::vector<GpgME::Key> &keys, unsigned int f)
{
    dump();
    if (!f || keys.empty()) {
        return;
    }
    for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
        if (!(f & concreteCryptoMessageFormats[i])) {
            continue;
        }
        auto pos = d->mFormatInfoMap.find(concreteCryptoMessageFormats[i]);
        if (pos == d->mFormatInfoMap.end()) {
            continue;
        }
        std::vector<SplitInfo> &v = pos->second.splitInfos;
        for (auto it = v.begin(); it != v.end(); ++it) {
            it->keys.insert(it->keys.end(), keys.begin(), keys.end());
        }
    }
    dump();
}

void Kleo::KeyResolver::dump() const
{
#ifndef NDEBUG
    if (d->mFormatInfoMap.empty()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Keyresolver: Format info empty";
    }
    for (auto it = d->mFormatInfoMap.begin(); it != d->mFormatInfoMap.end(); ++it) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Format info for " << Kleo::cryptoMessageFormatToString(it->first) << ":  Signing keys: ";
        for (auto sit = it->second.signKeys.begin(); sit != it->second.signKeys.end(); ++sit) {
            qCDebug(MESSAGECOMPOSER_LOG) << "  " << sit->shortKeyID() << " ";
        }
        unsigned int i = 0;
        for (auto sit = it->second.splitInfos.begin(), sitEnd = it->second.splitInfos.end(); sit != sitEnd; ++sit, ++i) {
            qCDebug(MESSAGECOMPOSER_LOG) << "  SplitInfo #" << i << " encryption keys: ";
            for (auto kit = sit->keys.begin(), sitEnd = sit->keys.end(); kit != sitEnd; ++kit) {
                qCDebug(MESSAGECOMPOSER_LOG) << "  " << kit->shortKeyID();
            }
            qCDebug(MESSAGECOMPOSER_LOG) << "  SplitInfo #" << i << " recipients: " << qPrintable(sit->recipients.join(QLatin1StringView(", ")));
        }
    }
#endif
}

Kleo::Result Kleo::KeyResolver::showKeyApprovalDialog(bool &finalySendUnencrypted)
{
    const bool showKeysForApproval = showApprovalDialog() || std::any_of(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), ApprovalNeeded)
        || std::any_of(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), ApprovalNeeded);

    if (!showKeysForApproval) {
        return Kleo::Ok;
    }

    std::vector<Kleo::KeyApprovalDialog::Item> items;
    items.reserve(d->mPrimaryEncryptionKeys.size() + d->mSecondaryEncryptionKeys.size());
    std::copy(d->mPrimaryEncryptionKeys.begin(), d->mPrimaryEncryptionKeys.end(), std::back_inserter(items));
    std::copy(d->mSecondaryEncryptionKeys.begin(), d->mSecondaryEncryptionKeys.end(), std::back_inserter(items));

    std::vector<GpgME::Key> senderKeys;
    senderKeys.reserve(d->mOpenPGPEncryptToSelfKeys.size() + d->mSMIMEEncryptToSelfKeys.size());
    std::copy(d->mOpenPGPEncryptToSelfKeys.begin(), d->mOpenPGPEncryptToSelfKeys.end(), std::back_inserter(senderKeys));
    std::copy(d->mSMIMEEncryptToSelfKeys.begin(), d->mSMIMEEncryptToSelfKeys.end(), std::back_inserter(senderKeys));

    KCursorSaver saver(Qt::WaitCursor);

    QPointer<Kleo::KeyApprovalDialog> dlg = new Kleo::KeyApprovalDialog(items, senderKeys);

    if (dlg->exec() == QDialog::Rejected) {
        delete dlg;
        return Kleo::Canceled;
    }

    items = dlg->items();
    senderKeys = dlg->senderKeys();
    const bool prefsChanged = dlg->preferencesChanged();
    delete dlg;

    if (prefsChanged) {
        for (uint i = 0, total = items.size(); i < total; ++i) {
            auto pref = lookupContactPreferences(items[i].address);
            pref.encryptionPreference = items[i].pref;
            pref.pgpKeyFingerprints.clear();
            pref.smimeCertFingerprints.clear();
            const std::vector<GpgME::Key> &keys = items[i].keys;
            for (auto it = keys.begin(), end = keys.end(); it != end; ++it) {
                if (it->protocol() == GpgME::OpenPGP) {
                    if (const char *fpr = it->primaryFingerprint()) {
                        pref.pgpKeyFingerprints.push_back(QLatin1StringView(fpr));
                    }
                } else if (it->protocol() == GpgME::CMS) {
                    if (const char *fpr = it->primaryFingerprint()) {
                        pref.smimeCertFingerprints.push_back(QLatin1StringView(fpr));
                    }
                }
            }
            saveContactPreference(items[i].address, pref);
        }
    }

    // show a warning if the user didn't select an encryption key for
    // herself:
    if (encryptToSelf() && senderKeys.empty()) {
        const QString msg = i18n(
            "You did not select an encryption key for yourself "
            "(encrypt to self). You will not be able to decrypt "
            "your own message if you encrypt it.");
        if (KMessageBox::warningContinueCancel(nullptr, msg, i18nc("@title:window", "Missing Key Warning"), KGuiItem(i18nc("@action:button", "&Encrypt")))
            == KMessageBox::Cancel) {
            return Kleo::Canceled;
        } else {
            mEncryptToSelf = false;
        }
    }

    // count empty key ID lists
    const unsigned int emptyListCount = std::count_if(items.begin(), items.end(), EmptyKeyList);

    // show a warning if the user didn't select an encryption key for
    // some of the recipients
    if (items.size() == emptyListCount) {
        const QString msg = (d->mPrimaryEncryptionKeys.size() + d->mSecondaryEncryptionKeys.size() == 1)
            ? i18n(
                "You did not select an encryption key for the "
                "recipient of this message; therefore, the message "
                "will not be encrypted.")
            : i18n(
                "You did not select an encryption key for any of the "
                "recipients of this message; therefore, the message "
                "will not be encrypted.");
        if (KMessageBox::warningContinueCancel(nullptr,
                                               msg,
                                               i18nc("@title:window", "Missing Key Warning"),
                                               KGuiItem(i18nc("@action:button", "Send &Unencrypted")))
            == KMessageBox::Cancel) {
            return Kleo::Canceled;
        }
        finalySendUnencrypted = true;
    } else if (emptyListCount > 0) {
        const QString msg = (emptyListCount == 1) ? i18n(
                                "You did not select an encryption key for one of "
                                "the recipients: this person will not be able to "
                                "decrypt the message if you encrypt it.")
                                                  : i18n(
                                                      "You did not select encryption keys for some of "
                                                      "the recipients: these persons will not be able to "
                                                      "decrypt the message if you encrypt it.");
        KCursorSaver saver(Qt::WaitCursor);
        if (KMessageBox::warningContinueCancel(nullptr, msg, i18nc("@title:window", "Missing Key Warning"), KGuiItem(i18nc("@action:button", "&Encrypt")))
            == KMessageBox::Cancel) {
            return Kleo::Canceled;
        }
    }

    std::transform(d->mPrimaryEncryptionKeys.begin(),
                   d->mPrimaryEncryptionKeys.end(),
                   items.begin(),
                   d->mPrimaryEncryptionKeys.begin(),
                   CopyKeysAndEncryptionPreferences);
    std::transform(d->mSecondaryEncryptionKeys.begin(),
                   d->mSecondaryEncryptionKeys.end(),
                   items.begin() + d->mPrimaryEncryptionKeys.size(),
                   d->mSecondaryEncryptionKeys.begin(),
                   CopyKeysAndEncryptionPreferences);

    d->mOpenPGPEncryptToSelfKeys.clear();
    d->mSMIMEEncryptToSelfKeys.clear();

    std::remove_copy_if(senderKeys.begin(),
                        senderKeys.end(),
                        std::back_inserter(d->mOpenPGPEncryptToSelfKeys),
                        NotValidTrustedOpenPGPEncryptionKey); // -= trusted (see above, too)?
    std::remove_copy_if(senderKeys.begin(),
                        senderKeys.end(),
                        std::back_inserter(d->mSMIMEEncryptToSelfKeys),
                        NotValidTrustedSMIMEEncryptionKey); // -= trusted (see above, too)?

    return Kleo::Ok;
}

std::vector<Kleo::KeyResolver::SplitInfo> Kleo::KeyResolver::encryptionItems(Kleo::CryptoMessageFormat f) const
{
    dump();
    auto it = d->mFormatInfoMap.find(f);
    return it != d->mFormatInfoMap.end() ? it->second.splitInfos : std::vector<SplitInfo>();
}

void Kleo::KeyResolver::setAutocryptEnabled(bool autocryptEnabled)
{
    d->mAutocryptEnabled = autocryptEnabled;
}

std::map<QByteArray, QString> Kleo::KeyResolver::useAutocrypt() const
{
    return d->mAutocryptMap;
}

void Kleo::KeyResolver::setAkonadiLookupEnabled(bool akonadiLoopkupEnabled)
{
    d->mAkonadiLookupEnabled = akonadiLoopkupEnabled;
}

std::vector<GpgME::Key> Kleo::KeyResolver::signingKeys(CryptoMessageFormat f) const
{
    dump();
    auto it = d->mFormatInfoMap.find(f);
    return it != d->mFormatInfoMap.end() ? it->second.signKeys : std::vector<GpgME::Key>();
}

//
//
// KeyResolverPrivate helper methods below:
//
//

std::vector<GpgME::Key> Kleo::KeyResolver::selectKeys(const QString &person, const QString &msg, const std::vector<GpgME::Key> &selectedKeys) const
{
    const bool opgp = containsOpenPGP(mCryptoMessageFormats);
    const bool x509 = containsSMIME(mCryptoMessageFormats);

    QPointer<Kleo::KeySelectionDialog> dlg = new Kleo::KeySelectionDialog(
        i18n("Encryption Key Selection"),
        msg,
        KEmailAddress::extractEmailAddress(person),
        selectedKeys,
        Kleo::KeySelectionDialog::ValidEncryptionKeys & ~(opgp ? 0 : Kleo::KeySelectionDialog::OpenPGPKeys) & ~(x509 ? 0 : Kleo::KeySelectionDialog::SMIMEKeys),
        true,
        true); // multi-selection and "remember choice" box

    if (dlg->exec() != QDialog::Accepted) {
        delete dlg;
        return {};
    }

    std::vector<GpgME::Key> keys = dlg->selectedKeys();
    keys.erase(std::remove_if(keys.begin(), keys.end(), NotValidEncryptionKey), keys.end());
    if (!keys.empty() && dlg->rememberSelection()) {
        setKeysForAddress(person, dlg->pgpKeyFingerprints(), dlg->smimeFingerprints());
    }

    delete dlg;
    return keys;
}

std::vector<GpgME::Key> Kleo::KeyResolver::getEncryptionKeys(const QString &person, bool quiet) const
{
    const QString address = canonicalAddress(person).toLower();

    // First look for this person's address in the address->key dictionary
    const QStringList fingerprints = keysForAddress(address);

    if (!fingerprints.empty()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Using encryption keys 0x" << fingerprints.join(QLatin1StringView(", 0x")) << "for" << person;
        std::vector<GpgME::Key> keys = lookup(fingerprints);
        if (!keys.empty()) {
            // Check if all of the keys are trusted and valid encryption keys
            if (std::any_of(keys.begin(), keys.end(),
                            NotValidTrustedEncryptionKey)) { // -= trusted?
                // not ok, let the user select: this is not conditional on !quiet,
                // since it's a bug in the configuration and the user should be
                // notified about it as early as possible:
                keys = selectKeys(person,
                                  i18nc("if in your language something like "
                                        "'certificate(s)' is not possible please "
                                        "use the plural in the translation",
                                        "There is a problem with the "
                                        "encryption certificate(s) for \"%1\".\n\n"
                                        "Please re-select the certificate(s) which should "
                                        "be used for this recipient.",
                                        person),
                                  keys);
            }
            bool canceled = false;
            keys = trustedOrConfirmed(keys, address, canceled);
            if (canceled) {
                return {};
            }

            if (!keys.empty()) {
                return keys;
            }
            // keys.empty() is considered cancel by callers, so go on
        }
    }

    // Now search all public keys for matching keys
    std::vector<GpgME::Key> matchingKeys = lookup(QStringList(address));
    matchingKeys.erase(std::remove_if(matchingKeys.begin(), matchingKeys.end(), NotValidEncryptionKey), matchingKeys.end());

    if (matchingKeys.empty() && d->mAutocryptEnabled) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Search in Autocrypt storage a key for " << address;
        const auto storage = MessageCore::AutocryptStorage::self();
        const auto recipient = storage->getRecipient(address.toUtf8());
        if (recipient) {
            const auto key = recipient->gpgKey();
            if (!key.isNull() && ValidEncryptionKey(key)) {
                qCDebug(MESSAGECOMPOSER_LOG) << "Found an valid autocrypt key.";
                matchingKeys.push_back(key);
            } else {
                const auto gossipKey = recipient->gossipKey();
                if (!gossipKey.isNull() && ValidEncryptionKey(gossipKey)) {
                    qCDebug(MESSAGECOMPOSER_LOG) << "Found an valid autocrypt gossip key.";
                    matchingKeys.push_back(gossipKey);
                }
            }
        }
        // Accept any autocrypt key, as the validility is not used in Autocrypt.
        if (matchingKeys.size() == 1) {
            if (recipient->prefer_encrypt()) {
                d->mContactPreferencesMap[address].encryptionPreference = AlwaysEncryptIfPossible;
            }
            d->mAutocryptMap[matchingKeys[0].primaryFingerprint()] = address;
            return matchingKeys;
        }
    }

    // if called with quite == true (from EncryptionPreferenceCounter), we only want to
    // check if there are keys for this recipients, not (yet) their validity, so
    // don't show the untrusted encryption key warning in that case
    bool canceled = false;
    if (!quiet) {
        matchingKeys = trustedOrConfirmed(matchingKeys, address, canceled);
    }
    if (canceled) {
        return {};
    }
    if (quiet || matchingKeys.size() == 1) {
        return matchingKeys;
    }

    // no match until now, or more than one key matches; let the user
    // choose the key(s)
    // FIXME: let user get the key from keyserver
    return trustedOrConfirmed(selectKeys(person,
                                         matchingKeys.empty() ? i18nc("if in your language something like "
                                                                      "'certificate(s)' is not possible please "
                                                                      "use the plural in the translation",
                                                                      "<qt>No valid and trusted encryption certificate was "
                                                                      "found for \"%1\".<br/><br/>"
                                                                      "Select the certificate(s) which should "
                                                                      "be used for this recipient. If there is no suitable certificate in the list "
                                                                      "you can also search for external certificates by clicking the button: "
                                                                      "search for external certificates.</qt>",
                                                                      person.toHtmlEscaped())
                                                              : i18nc("if in your language something like "
                                                                      "'certificate(s)' is not possible please "
                                                                      "use the plural in the translation",
                                                                      "More than one certificate matches \"%1\".\n\n"
                                                                      "Select the certificate(s) which should "
                                                                      "be used for this recipient.",
                                                                      person.toHtmlEscaped()),
                                         matchingKeys),
                              address,
                              canceled);
    // we can ignore 'canceled' here, since trustedOrConfirmed() returns
    // an empty vector when canceled == true, and we'd just do the same
}

std::vector<GpgME::Key> Kleo::KeyResolver::lookup(const QStringList &patterns, bool secret) const
{
    if (patterns.empty()) {
        return {};
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "( \"" << patterns.join(QLatin1StringView("\", \"")) << "\"," << secret << ")";
    std::vector<GpgME::Key> result;
    if (mCryptoMessageFormats & (InlineOpenPGPFormat | OpenPGPMIMEFormat)) {
        if (const QGpgME::Protocol *p = QGpgME::openpgp()) {
            std::unique_ptr<QGpgME::KeyListJob> job(p->keyListJob(false, false, true)); // use validating keylisting
            if (job.get()) {
                std::vector<GpgME::Key> keys;
                job->exec(patterns, secret, keys);
                result.insert(result.end(), keys.begin(), keys.end());
            }
        }
    }
    if (mCryptoMessageFormats & (SMIMEFormat | SMIMEOpaqueFormat)) {
        if (const QGpgME::Protocol *p = QGpgME::smime()) {
            std::unique_ptr<QGpgME::KeyListJob> job(p->keyListJob(false, false, true)); // use validating keylisting
            if (job.get()) {
                std::vector<GpgME::Key> keys;
                job->exec(patterns, secret, keys);
                result.insert(result.end(), keys.begin(), keys.end());
            }
        }
    }
    qCDebug(MESSAGECOMPOSER_LOG) << " returned" << result.size() << "keys";
    return result;
}

void Kleo::KeyResolver::addKeys(const std::vector<Item> &items, CryptoMessageFormat f)
{
    dump();
    for (auto it = items.begin(); it != items.end(); ++it) {
        SplitInfo si(QStringList(it->address));
        std::remove_copy_if(it->keys.begin(), it->keys.end(), std::back_inserter(si.keys), IsNotForFormat(f));
        dump();
        if (si.keys.empty()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Kleo::KeyResolver::addKeys(): Fix EncryptionFormatPreferenceCounter."
                                           << "It detected a common format, but the list of such keys for recipient \"" << it->address << "\" is empty!";
        }
        d->mFormatInfoMap[f].splitInfos.push_back(si);
    }
    dump();
}

void Kleo::KeyResolver::addKeys(const std::vector<Item> &items)
{
    dump();
    for (auto it = items.begin(); it != items.end(); ++it) {
        SplitInfo si(QStringList(it->address));
        CryptoMessageFormat f = AutoFormat;
        for (unsigned int i = 0; i < numConcreteCryptoMessageFormats; ++i) {
            const CryptoMessageFormat fmt = concreteCryptoMessageFormats[i];
            if ((fmt & it->format) && std::any_of(it->keys.begin(), it->keys.end(), IsForFormat(fmt))) {
                f = fmt;
                break;
            }
        }
        if (f == AutoFormat) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Something went wrong. Didn't find a format for \"" << it->address << "\"";
        } else {
            std::remove_copy_if(it->keys.begin(), it->keys.end(), std::back_inserter(si.keys), IsNotForFormat(f));
        }
        d->mFormatInfoMap[f].splitInfos.push_back(si);
    }
    dump();
}

MessageComposer::ContactPreference Kleo::KeyResolver::lookupContactPreferences(const QString &address) const
{
    const auto it = d->mContactPreferencesMap.find(address);
    if (it != d->mContactPreferencesMap.end()) {
        return it->second;
    }

    MessageComposer::ContactPreference pref;

    if (!d->mAkonadiLookupEnabled) {
        return pref;
    }

    auto job = new Akonadi::ContactSearchJob();
    job->setLimit(1);
    job->setQuery(Akonadi::ContactSearchJob::Email, address);
    job->exec();

    const KContacts::Addressee::List res = job->contacts();
    if (!res.isEmpty()) {
        KContacts::Addressee addr = res.at(0);
        pref.fillFromAddressee(addr);
    }

    const_cast<KeyResolver *>(this)->setContactPreferences(address, pref);

    return pref;
}

void Kleo::KeyResolver::setContactPreferences(const QString &address, const MessageComposer::ContactPreference &pref)
{
    d->mContactPreferencesMap.insert(std::make_pair(address, pref));
}

void Kleo::KeyResolver::saveContactPreference(const QString &email, const MessageComposer::ContactPreference &pref) const
{
    d->mContactPreferencesMap.insert(std::make_pair(email, pref));
    auto saveContactPreferencesJob = new MessageComposer::SaveContactPreferenceJob(email, pref);
    saveContactPreferencesJob->start();
}

QStringList Kleo::KeyResolver::keysForAddress(const QString &address) const
{
    if (address.isEmpty()) {
        return {};
    }
    const QString addr = canonicalAddress(address).toLower();
    const auto pref = lookupContactPreferences(addr);
    return pref.pgpKeyFingerprints + pref.smimeCertFingerprints;
}

void Kleo::KeyResolver::setKeysForAddress(const QString &address, const QStringList &pgpKeyFingerprints, const QStringList &smimeCertFingerprints) const
{
    if (address.isEmpty()) {
        return;
    }
    const QString addr = canonicalAddress(address).toLower();
    auto pref = lookupContactPreferences(addr);
    pref.pgpKeyFingerprints = pgpKeyFingerprints;
    pref.smimeCertFingerprints = smimeCertFingerprints;
    saveContactPreference(addr, pref);
}

bool Kleo::KeyResolver::encryptToSelf() const
{
    return mEncryptToSelf;
}

bool Kleo::KeyResolver::showApprovalDialog() const
{
    return mShowApprovalDialog;
}
