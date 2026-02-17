/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <KMime/Message>
#include <Libkleo/Enum>

namespace MessageComposer
{

/*! \class DraftEncryptionState
    \inheaderfile draftstatus.h
    \brief Manages the encryption state of a draft message.
    \inmodule MessageComposer

    This class stores and retrieves the encryption preference in the headers of a draft message.
*/
class MESSAGECOMPOSER_EXPORT DraftEncryptionState
{
public:
    /*! \brief Constructs a DraftEncryptionState for the given message. */
    explicit DraftEncryptionState(const std::shared_ptr<KMime::Message> &msg);

    /*! \brief Sets the encryption state in the message headers. */
    void setState(bool encrypt);

    /*! \brief Removes the encryption state from message headers. */
    void removeState();

    /*! \brief Returns the stored encryption state.
        \return True if encryption is enabled, false otherwise.
    */
    [[nodiscard]] bool encryptionState() const;

    /*! \brief Checks if encryption state is defined in the message.
        \return True if the state is defined, false otherwise.
    */
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

/*! \class DraftSignatureState
    \inheaderfile draftstatus.h
    \brief Manages the signature state of a draft message.
    \inmodule MessageComposer

    This class stores and retrieves the signing preference in the headers of a draft message.
*/
class MESSAGECOMPOSER_EXPORT DraftSignatureState
{
public:
    /*! \brief Constructs a DraftSignatureState for the given message. */
    explicit DraftSignatureState(const std::shared_ptr<KMime::Message> &msg);

    /*! \brief Sets the signature state in the message headers. */
    void setState(bool sign);

    /*! \brief Removes the signature state from message headers. */
    void removeState();

    /*! \brief Returns the stored signature state.
        \return True if signing is enabled, false otherwise.
    */
    [[nodiscard]] bool signState() const;

    /*! \brief Checks if signature state is defined in the message.
        \return True if the state is defined, false otherwise.
    */
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

/*! \class DraftCryptoMessageFormatState
    \inheaderfile draftstatus.h
    \brief Manages the crypto message format state of a draft message.
    \inmodule MessageComposer

    This class stores and retrieves the preferred cryptographic message format in message headers.
*/
class MESSAGECOMPOSER_EXPORT DraftCryptoMessageFormatState
{
public:
    /*! \brief Constructs a DraftCryptoMessageFormatState for the given message. */
    explicit DraftCryptoMessageFormatState(const std::shared_ptr<KMime::Message> &msg);

    /*! \brief Sets the crypto message format state in the message headers. */
    void setState(Kleo::CryptoMessageFormat cryptoMessageFormat);

    /*! \brief Removes the crypto message format state from message headers. */
    void removeState();

    /*! \brief Returns the stored crypto message format state.
        \return The crypto message format preference.
    */
    [[nodiscard]] Kleo::CryptoMessageFormat cryptoMessageFormatState() const;

    /*! \brief Checks if crypto message format state is defined in the message.
        \return True if the state is defined, false otherwise.
    */
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

/*! \brief Removes all draft crypto-related headers from a message.
    \param msg The message to clean up.
*/
void MESSAGECOMPOSER_EXPORT removeDraftCryptoHeaders(const std::shared_ptr<KMime::Message> &msg);
}
