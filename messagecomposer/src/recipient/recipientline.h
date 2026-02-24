/*
    SPDX-FileCopyrightText: 2010 Casey Link <unnamedrambler@gmail.com>
    SPDX-FileCopyrightText: 2009-2010 Klaralvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QSharedPointer>

#include "MessageComposer/Recipient"

#include <Libkdepim/MultiplyingLine>
#include <MessageComposer/ComposerLineEdit>

#include <QComboBox>

namespace MessageComposer
{
class RecipientComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit RecipientComboBox(QWidget *parent);

Q_SIGNALS:
    void rightPressed();

protected:
    void keyPressEvent(QKeyEvent *ev) override;
};
/*!
 * \class MessageComposer::RecipientLineEdit
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/RecipientLine
 * \brief The RecipientLineEdit class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineEdit : public MessageComposer::ComposerLineEdit
{
    Q_OBJECT
public:
    /*! \brief Constructs a RecipientLineEdit widget.
        \param parent The parent widget.
    */
    explicit RecipientLineEdit(QWidget *parent);

Q_SIGNALS:
    /*! \brief Emitted when the user requests to delete this line. */
    void deleteMe();
    /*! \brief Emitted when the user presses the left arrow key. */
    void leftPressed();
    /*! \brief Emitted when the user presses the right arrow key. */
    void rightPressed();

protected:
    /*! \brief Handles key press events for navigation and deletion.
        \param ev The key event.
    */
    void keyPressEvent(QKeyEvent *ev) override;
};

/*!
 * \class MessageComposer::RecipientLineNG
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/RecipientLine
 * \brief The RecipientLineNG class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineNG : public KPIM::MultiplyingLine
{
    Q_OBJECT
public:
    /*! \brief Constructs a RecipientLineNG widget.
        \param parent The parent widget.
    */
    explicit RecipientLineNG(QWidget *parent);
    /*! \brief Destroys the RecipientLineNG. */
    ~RecipientLineNG() override = default;

    /*! \brief Activates the recipient line for input. */
    void activate() override;
    /*! \brief Returns whether the recipient line is currently active. */
    [[nodiscard]] bool isActive() const override;

    /*! \brief Returns whether the recipient line is empty. */
    [[nodiscard]] bool isEmpty() const override;
    /*! \brief Clears the contents of the recipient line. */
    void clear() override;
    /*! \brief Returns whether this line can be deleted. */
    [[nodiscard]] bool canDeleteLineEdit() const override;

    /*! \brief Returns whether the recipient line content has been modified. */
    [[nodiscard]] bool isModified() const override;
    /*! \brief Clears the modified flag. */
    void clearModified() override;

    /*! \brief Returns the recipient data for this line. */
    [[nodiscard]] KPIM::MultiplyingLineData::Ptr data() const override;
    /*! \brief Sets the recipient data for this line.
        \param data The recipient data pointer.
    */
    void setData(const KPIM::MultiplyingLineData::Ptr &data) override;

    /*! \brief Sets up the tab order for recipient input.
        \param previous The previous widget in the tab order.
    */
    void fixTabOrder(QWidget *previous) override;
    /*! \brief Returns the widget to tab out to next. */
    [[nodiscard]] QWidget *tabOut() const override;

    /*! \brief Sets the completion mode for recipient address completion.
        \param mode The completion mode to use.
    */
    void setCompletionMode(KCompletion::CompletionMode mode) override;

    /*! \brief Sets the column width for the recipient line.
        \param w The desired width in pixels.
        \return The actual width set.
    */
    [[nodiscard]] int setColumnWidth(int w) override;

    // recipient specific methods
    /*! \brief Returns the number of recipients in this line. */
    [[nodiscard]] int recipientsCount() const;

    /*! \brief Sets the type of recipients (To, Cc, Bcc).
        \param type The recipient type.
    */
    void setRecipientType(Recipient::Type);
    /*! \brief Returns the type of recipients in this line. */
    [[nodiscard]] Recipient::Type recipientType() const;
    /*! \brief Returns the recipient data object. */
    [[nodiscard]] QSharedPointer<Recipient> recipient() const;

    /*! \brief Sets the icon displayed for this recipient line.
        \param icon The icon to display.
        \param tooltip Optional tooltip text for the icon.
    */
    void setIcon(const QIcon &icon, const QString &tooltip = QString());

    /**
     * Sets the config file used for storing recent addresses.
     */
    void setRecentAddressConfig(KConfig *config);

    /*! \brief Enables or disables index search for recipient lookup.
        \param enableIndexSearch True to enable index search, false otherwise.
    */
    void setEnableIndexSearch(bool enableIndexSearch);
    /*! \brief Returns whether index search is enabled. */
    [[nodiscard]] bool enableIndexSearch() const;

    /*! \brief Enables or disables Akonadi search for recipient lookup.
        \param enableAkonadiSearch True to enable Akonadi search, false otherwise.
    */
    void setEnableAkonadiSearch(bool enableAkonadiSearch);
    /*! \brief Returns whether Akonadi search is enabled. */
    [[nodiscard]] bool enableAkonadiSearch() const;

    /*! \brief Returns the raw recipient data as a string. */
    [[nodiscard]] QString rawData() const;

Q_SIGNALS:
    /*! \brief Emitted when the recipient type is modified.
        \param line The recipient line that was modified.
    */
    void typeModified(MessageComposer::RecipientLineNG *);
    /*! \brief Emitted when a recipient should be added.
        \param line The recipient line.
        \param recipient The recipient address to add.
    */
    void addRecipient(MessageComposer::RecipientLineNG *, const QString &);
    /*! \brief Emitted when the recipient count changes. */
    void countChanged();
    /*! \brief Emitted when the icon is clicked. */
    void iconClicked();
    /*! \brief Emitted when the active state changes. */
    void activeChanged();

protected Q_SLOTS:
    /*! \brief Called when editing of the recipient line is finished. */
    void slotEditingFinished();
    /*! \brief Called when the recipient type has been modified. */
    void slotTypeModified();
    /*! \brief Analyzes the recipient line content and updates the data.
        \param text The text content to analyze.
    */
    void analyzeLine(const QString &);

protected:
    /*! \brief Filters events for the recipient line widgets.
        \param watched The object that the event is coming from.
        \param event The event to process.
        \return True if the event was handled, false otherwise.
    */
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    MESSAGECOMPOSER_NO_EXPORT void dataFromFields();
    MESSAGECOMPOSER_NO_EXPORT void fieldsFromData();
    MESSAGECOMPOSER_NO_EXPORT void slotAddRecipient(const QString &);
    RecipientComboBox *const mCombo;
    RecipientLineEdit *const mEdit;
    int mRecipientsCount = 0;
    bool mModified = false;
    QSharedPointer<Recipient> mData;
};
}
