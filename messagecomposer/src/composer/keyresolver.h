/*  -*- c++ -*-
    keyresolver.h

    This file is part of libkleopatra, the KDE keymanagement library
    SPDX-FileCopyrightText: 2004 Klarälvdalens Datakonsult AB

    Based on kpgp.h
    Copyright (C) 2001,2002 the KPGP authors
    See file libkdenetwork/AUTHORS.kpgp for details

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <Libkleo/Enum>
#include <Libkleo/KeyApprovalDialog>

#include <gpgme++/key.h>

#include <vector>

#include <QStringList>

namespace Kleo
{
enum Result { Failure = 0, Ok = 1, Canceled = 2 };
/**
     \short A class to resolve signing/encryption keys w.r.t. per-recipient preferences

     \section Step 1: Set the information needed

     The constructor takes some basic options as arguments, such as
     whether or not encryption was actually requested. Recipient and
     sender information is then set by using \c
     setEncryptToSelfKeys(), \c setSigningKeys(), \c
     setPrimaryRecipients() (To/Cc) and \c setSecondaryRecipients()
     (Bcc).

     \section Step 2: Lookup and check per-recipient crypto preferences / Opportunistic Encryption

     First, \c checkSigningPreferences() goes through all recipient's
     signing preferences, to determine whether or not to sign. It also
     takes into account the available signing keys and whether or not
     the user explicitly requested signing.

     \c checkEncryptionPreferences() does the same for encryption
     preferences. If opportunistic encryption is enabled, recipients
     without encryption preferences set are treated as if they had a
     preference of \c AskWheneverPossible.

     In both cases an Action code is returned, with the following
     meanings:

     <dl><dt>Conflict</dt><dd>A conflict was detected. E.g. one
     recipient's preference was set to "always encrypt", while another
     one's preference was set to "never encrypt". You should ask the
     user what to do.</dd></dt>

     <dt>DoIt, DontDoIt</dt><dd>Do/Don't sign/encrypt</dd>

     <dt>Ask</dt><dd>(Some) crypto preferences request to prompt the
     user, so do it.</dd>

     <dt>Impossible</dt><dd>Signing or encryption is impossible,
     e.g. due to missing keys or unsupported formats.</dd> </dl>

     \section Step 3: Resolve all keys.

     In case signing or encryption was implicitly or explicitly
     requested by the user, \c resolveAllKeys() tries to find signing
     keys for each required format, as well as encryption keys for all
     recipients (incl. the sender, if encrypt-to-self is set).

     \section Step 4: Get signing keys.

     If, after key resolving, signing is still requested and
     apparently possible, you can get the result of all this by
     iterating over the available message formats and retrieving the
     set of signing keys to use with a call to \c signingKeys().

     \section Step 5: Get encryption key sets.

     If after key resolving, encryption is still requested and
     apparently possible, you can get the result of all this by
     calling \c encryptionItems() with the current message format at
     hand as its argument.

     This will return a list of recipient-list/key-list pairs that
     each describe a copy of the (possibly signed) message to be
     encrypted independantly.

     Note that it's only necessary to sign the message once for each
     message format, although it might be necessary to create more
     than one message when encrypting. This is because encryption
     allows the recipients to learn about the other recipients the
     message was encrypted to, so each secondary (BCC) recipient need
     a copy of it's own to hide the other secondary recipients.
    */

class MESSAGECOMPOSER_EXPORT KeyResolver
{
public:
    KeyResolver(bool encToSelf,
                bool showApproval,
                bool oppEncryption,
                unsigned int format,
                int encrKeyNearExpiryThresholdDays,
                int signKeyNearExpiryThresholdDays,
                int encrRootCertNearExpiryThresholdDays,
                int signRootCertNearExpiryThresholdDays,
                int encrChainCertNearExpiryThresholdDays,
                int signChainCertNearExpiryThresholdDays);

    ~KeyResolver();

    struct ContactPreferences {
        ContactPreferences();
        Kleo::EncryptionPreference encryptionPreference;
        Kleo::SigningPreference signingPreference;
        Kleo::CryptoMessageFormat cryptoMessageFormat;
        QStringList pgpKeyFingerprints;
        QStringList smimeCertFingerprints;
    };

    struct Item : public KeyApprovalDialog::Item {
        Item()
            : KeyApprovalDialog::Item()
            , signPref(UnknownSigningPreference)
            , format(AutoFormat)
            , needKeys(true)
        {
        }

        Item(const QString &a, EncryptionPreference e, SigningPreference s, CryptoMessageFormat f)
            : KeyApprovalDialog::Item(a, std::vector<GpgME::Key>(), e)
            , signPref(s)
            , format(f)
            , needKeys(true)
        {
        }

        Item(const QString &a, const std::vector<GpgME::Key> &k, EncryptionPreference e, SigningPreference s, CryptoMessageFormat f)
            : KeyApprovalDialog::Item(a, k, e)
            , signPref(s)
            , format(f)
            , needKeys(false)
        {
        }

        SigningPreference signPref;
        CryptoMessageFormat format;
        bool needKeys;
    };

    /**
       Set the fingerprints of keys to be used for encrypting to
       self. Also looks them up and complains if they're not usable or
       found.
    */
    Q_REQUIRED_RESULT Kleo::Result setEncryptToSelfKeys(const QStringList &fingerprints);
    /**
        Set the fingerprints of keys to be used for signing. Also
        looks them up and complains if they're not usable or found.
    */
    Q_REQUIRED_RESULT Kleo::Result setSigningKeys(const QStringList &fingerprints);
    /**
       Set the list of primary (To/CC) recipient addresses. Also looks
       up possible keys, but doesn't interact with the user.
    */
    void setPrimaryRecipients(const QStringList &addresses);
    /**
       Set the list of secondary (BCC) recipient addresses. Also looks
       up possible keys, but doesn't interact with the user.
    */
    void setSecondaryRecipients(const QStringList &addresses);

    /**
       Determine whether to sign or not, depending on the
       per-recipient signing preferences, as well as the availability
       of usable signing keys.
    */
    Q_REQUIRED_RESULT Action checkSigningPreferences(bool signingRequested) const;
    /**
       Determine whether to encrypt or not, depending on the
       per-recipient encryption preferences, as well as the availability
       of usable encryption keys.
    */
    Q_REQUIRED_RESULT Action checkEncryptionPreferences(bool encryptionRequested) const;

    /**
       Queries the user for missing keys and displays a key approval
       dialog if needed.
    */
    Q_REQUIRED_RESULT Kleo::Result resolveAllKeys(bool &signingRequested, bool &encryptionRequested);

    /**
       @return the signing keys to use (if any) for the given message
       format.
    */
    Q_REQUIRED_RESULT std::vector<GpgME::Key> signingKeys(CryptoMessageFormat f) const;

    struct SplitInfo {
        SplitInfo()
        {
        }

        SplitInfo(const QStringList &r)
            : recipients(r)
        {
        }

        SplitInfo(const QStringList &r, const std::vector<GpgME::Key> &k)
            : recipients(r)
            , keys(k)
        {
        }

        QStringList recipients;
        std::vector<GpgME::Key> keys;
    };
    /** @return the found distinct sets of items for format \a f.  The
        returned vector will contain more than one item only if
        secondary recipients have been specified.
    */
    Q_REQUIRED_RESULT std::vector<SplitInfo> encryptionItems(CryptoMessageFormat f) const;

    std::vector<GpgME::Key> encryptToSelfKeysFor(CryptoMessageFormat f) const;

    /** If Autocrypt keys are used to find valid PGP Keys
     */
    void setAutocryptEnabled(bool autocryptEnabled);

    std::map<QByteArray, QString> useAutocrypt() const;

private:
    void dump() const;
    std::vector<Item> getEncryptionItems(const QStringList &recipients);
    std::vector<GpgME::Key> getEncryptionKeys(const QString &recipient, bool quiet) const;

    Kleo::Result showKeyApprovalDialog(bool &finalySendUnencrypted);

    bool encryptionPossible() const;
    bool signingPossible() const;
    Kleo::Result resolveEncryptionKeys(bool signingRequested, bool &finalySendUnencrypted);
    Kleo::Result resolveSigningKeysForEncryption();
    Kleo::Result resolveSigningKeysForSigningOnly();
    Kleo::Result checkKeyNearExpiry(const GpgME::Key &key,
                                    const char *dontAskAgainName,
                                    bool mine,
                                    bool sign,
                                    bool ca = false,
                                    int recurse_limit = 100,
                                    const GpgME::Key &orig_key = GpgME::Key::null) const;
    void collapseAllSplitInfos();
    void addToAllSplitInfos(const std::vector<GpgME::Key> &keys, unsigned int formats);
    void addKeys(const std::vector<Item> &items, CryptoMessageFormat f);
    void addKeys(const std::vector<Item> &items);
    QStringList allRecipients() const;
    std::vector<GpgME::Key> signingKeysFor(CryptoMessageFormat f) const;

    std::vector<GpgME::Key> lookup(const QStringList &patterns, bool secret = false) const;

    std::vector<GpgME::Key>
    selectKeys(const QString &person, const QString &msg, const std::vector<GpgME::Key> &selectedKeys = std::vector<GpgME::Key>()) const;

    QStringList keysForAddress(const QString &address) const;
    void setKeysForAddress(const QString &address, const QStringList &pgpKeyFingerprints, const QStringList &smimeCertFingerprints) const;

    bool encryptToSelf() const;
    bool showApprovalDialog() const;

    int encryptKeyNearExpiryWarningThresholdInDays() const;
    int signingKeyNearExpiryWarningThresholdInDays() const;

    int encryptRootCertNearExpiryWarningThresholdInDays() const;
    int signingRootCertNearExpiryWarningThresholdInDays() const;

    int encryptChainCertNearExpiryWarningThresholdInDays() const;
    int signingChainCertNearExpiryWarningThresholdInDays() const;

    ContactPreferences lookupContactPreferences(const QString &address) const;
    void saveContactPreference(const QString &email, const ContactPreferences &pref) const;

private:
    class EncryptionPreferenceCounter;
    friend class ::Kleo::KeyResolver::EncryptionPreferenceCounter;
    class SigningPreferenceCounter;
    friend class ::Kleo::KeyResolver::SigningPreferenceCounter;

    struct Private;
    Private *const d;

    bool mEncryptToSelf;
    const bool mShowApprovalDialog : 1;
    const bool mOpportunisticEncyption : 1;
    const unsigned int mCryptoMessageFormats;

    const int mEncryptKeyNearExpiryWarningThreshold;
    const int mSigningKeyNearExpiryWarningThreshold;
    const int mEncryptRootCertNearExpiryWarningThreshold;
    const int mSigningRootCertNearExpiryWarningThreshold;
    const int mEncryptChainCertNearExpiryWarningThreshold;
    const int mSigningChainCertNearExpiryWarningThreshold;
};
} // namespace Kleo

