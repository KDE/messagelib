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

#include <QSharedPointer>
#include <QStringList>

namespace MessageComposer
{
class ContactPreference;
}

namespace Kleo
{
class ExpiryChecker;

enum Result {
    Failure = 0,
    Ok = 1,
    Canceled = 2,
};
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
     encrypted independently.

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
    KeyResolver(bool encToSelf, bool showApproval, bool oppEncryption, unsigned int format, const std::shared_ptr<Kleo::ExpiryChecker> &expiryChecker);

    ~KeyResolver();

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
    [[nodiscard]] Kleo::Result setEncryptToSelfKeys(const QStringList &fingerprints);
    /**
        Set the fingerprints of keys to be used for signing. Also
        looks them up and complains if they're not usable or found.
    */
    [[nodiscard]] Kleo::Result setSigningKeys(const QStringList &fingerprints);
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
    [[nodiscard]] Action checkSigningPreferences(bool signingRequested) const;
    /**
       Determine whether to encrypt or not, depending on the
       per-recipient encryption preferences, as well as the availability
       of usable encryption keys.
    */
    [[nodiscard]] Action checkEncryptionPreferences(bool encryptionRequested) const;

    /**
       Queries the user for missing keys and displays a key approval
       dialog if needed.
    */
    [[nodiscard]] Kleo::Result resolveAllKeys(bool &signingRequested, bool &encryptionRequested);

    /**
       @return the signing keys to use (if any) for the given message
       format.
    */
    [[nodiscard]] std::vector<GpgME::Key> signingKeys(CryptoMessageFormat f) const;

    struct SplitInfo {
        SplitInfo() = default;

        explicit SplitInfo(const QStringList &r)
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
    [[nodiscard]] std::vector<SplitInfo> encryptionItems(CryptoMessageFormat f) const;

    [[nodiscard]] std::vector<GpgME::Key> encryptToSelfKeysFor(CryptoMessageFormat f) const;

    /** If Autocrypt keys are used to find valid PGP Keys
     */
    void setAutocryptEnabled(bool autocryptEnabled);

    [[nodiscard]] std::map<QByteArray, QString> useAutocrypt() const;

    /** Disable ContactSearchJob in KeyResolver.
        A usecase is that ests won't fireup an Akonadi instance only for this feature.
        @default is true: The ContactSearchJob is executed.
     */
    void setAkonadiLookupEnabled(bool akonadiLookupEnabled);

    /** Sets crypto preferences for given email address.
     * This is an alternative to setting crypto preferences for a contact when Akonadi
     * lookup is disabled - useful mostly for testing cases when it's not possible to
     * index contacts on demand.
     */
    void setContactPreferences(const QString &address, const MessageComposer::ContactPreference &preference);

private:
    void dump() const;
    [[nodiscard]] std::vector<Item> getEncryptionItems(const QStringList &recipients);
    [[nodiscard]] std::vector<GpgME::Key> getEncryptionKeys(const QString &recipient, bool quiet) const;

    [[nodiscard]] Kleo::Result showKeyApprovalDialog(bool &finalySendUnencrypted);

    [[nodiscard]] bool encryptionPossible() const;
    [[nodiscard]] bool signingPossible() const;
    [[nodiscard]] Kleo::Result resolveEncryptionKeys(bool signingRequested, bool &finalySendUnencrypted);
    [[nodiscard]] Kleo::Result resolveSigningKeysForEncryption();
    [[nodiscard]] Kleo::Result resolveSigningKeysForSigningOnly();
    void collapseAllSplitInfos();
    void addToAllSplitInfos(const std::vector<GpgME::Key> &keys, unsigned int formats);
    void addKeys(const std::vector<Item> &items, CryptoMessageFormat f);
    void addKeys(const std::vector<Item> &items);
    [[nodiscard]] QStringList allRecipients() const;
    [[nodiscard]] std::vector<GpgME::Key> signingKeysFor(CryptoMessageFormat f) const;

    [[nodiscard]] std::vector<GpgME::Key> lookup(const QStringList &patterns, bool secret = false) const;

    [[nodiscard]] std::vector<GpgME::Key>
    selectKeys(const QString &person, const QString &msg, const std::vector<GpgME::Key> &selectedKeys = std::vector<GpgME::Key>()) const;

    [[nodiscard]] QStringList keysForAddress(const QString &address) const;
    void setKeysForAddress(const QString &address, const QStringList &pgpKeyFingerprints, const QStringList &smimeCertFingerprints) const;

    [[nodiscard]] bool encryptToSelf() const;
    [[nodiscard]] bool showApprovalDialog() const;

    [[nodiscard]] MessageComposer::ContactPreference lookupContactPreferences(const QString &address) const;
    void saveContactPreference(const QString &email, const MessageComposer::ContactPreference &pref) const;

private:
    class EncryptionPreferenceCounter;
    friend class ::Kleo::KeyResolver::EncryptionPreferenceCounter;
    class SigningPreferenceCounter;
    friend class ::Kleo::KeyResolver::SigningPreferenceCounter;

    struct KeyResolverPrivate;
    std::unique_ptr<KeyResolverPrivate> const d;

    bool mEncryptToSelf;
    const bool mShowApprovalDialog : 1;
    const bool mOpportunisticEncyption : 1;
    const unsigned int mCryptoMessageFormats;
};
} // namespace Kleo
