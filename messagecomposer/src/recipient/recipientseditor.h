/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    Refactored from earlier code by:
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messagecomposer_export.h"

#include "recipientline.h"

#include <Libkdepim/MultiplyingLineEditor>

namespace KMime
{
namespace Types
{
class Mailbox;
}
}
namespace MessageComposer
{
/*!
 * \class MessageComposer::RecipientLineFactory
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/RecipientsEditor
 * \brief The RecipientLineFactory class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineFactory : public KPIM::MultiplyingLineFactory
{
    Q_OBJECT
public:
    /*! \brief Constructs a RecipientLineFactory.
        \param parent The parent object.
    */
    explicit RecipientLineFactory(QObject *parent);
    KPIM::MultiplyingLine *newLine(QWidget *parent) override;
    int maximumRecipients() override;
};

class RecipientsPicker;
class RecipientsEditorPrivate;
/*!
 * \class MessageComposer::RecipientsEditor
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/RecipientsEditor
 * \brief The RecipientsEditor class
 */
class MESSAGECOMPOSER_EXPORT RecipientsEditor : public KPIM::MultiplyingLineEditor
{
    Q_OBJECT
public:
    /*! \brief Constructs a RecipientsEditor.
        \param parent The parent widget.
    */
    explicit RecipientsEditor(QWidget *parent = nullptr);
    /*! \brief Constructs a RecipientsEditor with a custom line factory.
        \param lineFactory The factory for creating recipient lines.
        \param parent The parent widget.
    */
    explicit RecipientsEditor(RecipientLineFactory *lineFactory, QWidget *parent = nullptr);
    /*! \brief Destroys the RecipientsEditor. */
    ~RecipientsEditor() override;

    /*! \brief Returns the list of all recipients in the editor. */
    [[nodiscard]] Recipient::List recipients() const;
    /*! \brief Returns the currently active recipient line. */
    [[nodiscard]] QSharedPointer<Recipient> activeRecipient() const;

    /*! \brief Returns the recipients picker dialog. */
    [[nodiscard]] MessageComposer::RecipientsPicker *picker() const;

    /*! \brief Sets the recipients from a list of mailboxes.
        \param mailboxes The list of mailboxes to add.
        \param type The recipient type (To, Cc, Bcc).
        \return True if successful.
    */
    bool setRecipientString(const QList<KMime::Types::Mailbox> &mailboxes, Recipient::Type);
    /*! \brief Returns the recipient string for the given type.
        \param type The recipient type.
        \return The recipients as a formatted string.
    */
    [[nodiscard]] QString recipientString(Recipient::Type) const;
    /*! \brief Returns the list of recipient strings for the given type.
        \param type The recipient type.
        \return The recipients as a string list.
    */
    [[nodiscard]] QStringList recipientStringList(Recipient::Type) const;

    /** Adds a recipient (or multiple recipients) to one line of the editor.
        @param recipient The recipient(s) you want to add.
        @param type      The recipient type.
    */
    bool addRecipient(const QString &recipient, Recipient::Type type);

    /** Removes the recipient provided it can be found and has the given type.
        @param recipient The recipient(s) you want to remove.
        @param type      The recipient type.
    */
    void removeRecipient(const QString &recipient, Recipient::Type type);

    /**
     * Sets the config file used for storing recent addresses.
     */
    void setRecentAddressConfig(KConfig *config);

public:
    /*! \brief Opens the recipient picker dialog. */
    void selectRecipients();
    /*! \brief Saves the current recipients as a distribution list. */
    void saveDistributionList();

protected Q_SLOTS:
    friend class RecipientLineFactory;

    void slotPickedRecipient(const MessageComposer::Recipient &, bool &tooManyAddress);
    void slotLineAdded(KPIM::MultiplyingLine *);
    void slotLineDeleted(int pos);
    void slotCalculateTotal();
    void addRecipient(MessageComposer::RecipientLineNG *, const QString &);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void focusInRecipientLineEdit();

private:
    std::unique_ptr<RecipientsEditorPrivate> const d;
};
}
