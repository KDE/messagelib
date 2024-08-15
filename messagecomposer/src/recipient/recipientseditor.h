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
/**
 * @brief The RecipientLineFactory class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineFactory : public KPIM::MultiplyingLineFactory
{
    Q_OBJECT
public:
    explicit RecipientLineFactory(QObject *parent);
    KPIM::MultiplyingLine *newLine(QWidget *parent) override;
    int maximumRecipients() override;
};

class RecipientsPicker;
class RecipientsEditorPrivate;
/**
 * @brief The RecipientsEditor class
 */
class MESSAGECOMPOSER_EXPORT RecipientsEditor : public KPIM::MultiplyingLineEditor
{
    Q_OBJECT
public:
    explicit RecipientsEditor(QWidget *parent = nullptr);
    explicit RecipientsEditor(RecipientLineFactory *lineFactory, QWidget *parent = nullptr);
    ~RecipientsEditor() override;

    [[nodiscard]] Recipient::List recipients() const;
    [[nodiscard]] QSharedPointer<Recipient> activeRecipient() const;

    [[nodiscard]] MessageComposer::RecipientsPicker *picker() const;

    bool setRecipientString(const QList<KMime::Types::Mailbox> &mailboxes, Recipient::Type);
    [[nodiscard]] QString recipientString(Recipient::Type) const;
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

public Q_SLOTS:
    void selectRecipients();
    void saveDistributionList();

protected Q_SLOTS:
    friend class RecipientLineFactory;

    void slotPickedRecipient(const Recipient &, bool &tooManyAddress);
    void slotLineAdded(KPIM::MultiplyingLine *);
    void slotLineDeleted(int pos);
    void slotCalculateTotal();
    void addRecipient(RecipientLineNG *, const QString &);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void focusInRecipientLineEdit();

private:
    std::unique_ptr<RecipientsEditorPrivate> const d;
};
}
