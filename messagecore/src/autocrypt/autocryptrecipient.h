/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include <KMime/Headers>

#include <gpgme++/data.h>

#include <QJsonDocument>

namespace MessageCore
{
class AutocryptRecipientPrivate;
class AutocryptStorage;
class HeaderMixupNodeHelper;

/*!
 * \class MessageCore::AutocryptRecipient
 * \inmodule MessageCore
 * \inheaderfile MessageCore/AutocryptRecipient
 */
class MESSAGECORE_EXPORT AutocryptRecipient
{
public:
    /*!
     * Defines a pointer to an AutocryptRecipient object.
     */
    using Ptr = QSharedPointer<AutocryptRecipient>;
    /*!
     * Creates a new AutocryptRecipient.
     */
    AutocryptRecipient();
    /*!
     * Destroys the AutocryptRecipient.
     */
    ~AutocryptRecipient();
    /*!
     * Updates the recipient information from a message.
     *
     * \a mixup The header mixup helper to extract information from.
     * \a header The autocrypt header to process.
     */
    void updateFromMessage(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);
    /*!
     * Updates the recipient information from a gossip header.
     *
     * \a mixup The header mixup helper to extract information from.
     * \a header The gossip header to process.
     */
    void updateFromGossip(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);

    /*!
     * Converts the recipient information to JSON format.
     *
     * \a format The JSON format to use.
     */
    [[nodiscard]] QByteArray toJson(QJsonDocument::JsonFormat format) const;
    /*!
     * Loads the recipient information from JSON data.
     *
     * \a json The JSON data to load.
     */
    void fromJson(const QByteArray &json);

    /*!
     * Returns whether the recipient information has changed.
     */
    [[nodiscard]] bool hasChanged() const;
    /*!
     * Sets the changed flag for the recipient.
     *
     * \a changed Whether the recipient has changed.
     */
    void setChangedFlag(bool changed);

    /*!
     * Returns the email address of the recipient.
     */
    [[nodiscard]] QByteArray addr() const;
    /*!
     * Returns the bad user agent information.
     */
    [[nodiscard]] QByteArray bad_user_agent() const;

    /*!
     * Returns the last time the recipient was seen.
     */
    [[nodiscard]] QDateTime last_seen() const;
    /*!
     * Returns the autocrypt timestamp.
     */
    [[nodiscard]] QDateTime autocrypt_timestamp() const;
    /*!
     * Returns the counting start timestamp.
     */
    [[nodiscard]] QDateTime counting_since() const;
    /*!
     * Returns the gossip timestamp.
     */
    [[nodiscard]] QDateTime gossip_timestamp() const;

    /*!
     * Returns the count of messages with autocrypt headers.
     */
    [[nodiscard]] int count_have_ach() const;
    /*!
     * Returns the count of messages without autocrypt headers.
     */
    [[nodiscard]] int count_no_ach() const;

    /*!
     * Returns whether the recipient prefers encryption.
     */
    [[nodiscard]] bool prefer_encrypt() const;

    /*!
     * Returns the GPG key for this recipient.
     */
    [[nodiscard]] GpgME::Key gpgKey() const;
    /*!
     * Returns the GPG key data as a byte array.
     */
    [[nodiscard]] QByteArray gpgKeydata() const;

    /*!
     * Returns the gossip GPG key for this recipient.
     */
    [[nodiscard]] GpgME::Key gossipKey() const;
    /*!
     * Returns the gossip GPG key data as a byte array.
     */
    [[nodiscard]] QByteArray gossipKeydata() const;

private:
    std::unique_ptr<AutocryptRecipientPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptStorage;
};
}
