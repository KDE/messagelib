/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    Based in kmail/recipientseditor.h/cpp
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH LicenseRef-Qt-Commercial-exception-1.0
*/

#pragma once

#include "messagecomposer_export.h"

#include <Libkdepim/MultiplyingLine>

#include <Libkleo/Enum>
#include <gpgme++/key.h>

#include <QSharedPointer>
#include <QString>
namespace MessageComposer
{
/** Represents a mail recipient. */
class RecipientPrivate;
/*!
 * \class MessageComposer::Recipient
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/Recipient
 * \brief The Recipient class
 */
class MESSAGECOMPOSER_EXPORT Recipient : public KPIM::MultiplyingLineData
{
public:
    using Ptr = QSharedPointer<Recipient>;
    using List = QList<Recipient::Ptr>;

    enum Type : uint8_t {
        To,
        Cc,
        Bcc,
        ReplyTo,
        Undefined,
    };

    /*! \brief Constructs a Recipient.
        \param email The email address of the recipient (optional).
        \param type The type of recipient (To, Cc, Bcc, etc.).
    */
    Recipient(const QString &email = QString(), Type type = To); // krazy:exclude=explicit
    /*! \brief Destroys the Recipient. */
    ~Recipient() override;
    /*! \brief Sets the type of this recipient.
        \param type The recipient type.
    */
    void setType(Type type);
    /*! \brief Returns the type of this recipient. */
    [[nodiscard]] Type type() const;

    /*! \brief Sets the email address of this recipient.
        \param email The email address.
    */
    void setEmail(const QString &email);
    /*! \brief Returns the email address of this recipient. */
    [[nodiscard]] QString email() const;

    /*! \brief Sets the display name of this recipient.
        \param name The recipient's name.
    */
    void setName(const QString &name);
    /*! \brief Returns the display name of this recipient. */
    [[nodiscard]] QString name() const;

    /*! \brief Returns whether this recipient is empty. */
    [[nodiscard]] bool isEmpty() const override;
    /*! \brief Clears all data from this recipient. */
    void clear() override;

    /*! \brief Converts a recipient type enum to its numeric ID.
        \param type The recipient type.
        \return The numeric ID for the type.
    */
    [[nodiscard]] static int typeToId(Type type);
    /*! \brief Converts a numeric ID back to a recipient type.
        \param id The numeric ID.
        \return The recipient type.
    */
    [[nodiscard]] static Type idToType(int id);

    /*! \brief Returns the localized label for this recipient type. */
    [[nodiscard]] QString typeLabel() const;
    /*! \brief Returns the localized label for the given recipient type.
        \param type The recipient type.
        \return The localized label.
    */
    [[nodiscard]] static QString typeLabel(Type type);
    /*! \brief Returns all localized labels for all recipient types. */
    [[nodiscard]] static QStringList allTypeLabels();

    /*! \brief Sets the encryption action for this recipient.
        \param action The encryption action to use.
    */
    void setEncryptionAction(const Kleo::Action action);
    /*! \brief Returns the encryption action for this recipient. */
    [[nodiscard]] Kleo::Action encryptionAction() const;

    /*! \brief Sets the OpenPGP key for this recipient.
        \param key The OpenPGP key.
    */
    void setKey(const GpgME::Key &key);
    /*! \brief Returns the OpenPGP key for this recipient. */
    [[nodiscard]] GpgME::Key key() const;

private:
    std::unique_ptr<RecipientPrivate> const d;
};
}
