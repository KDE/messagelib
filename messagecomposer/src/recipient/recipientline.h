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
/**
 * @brief The RecipientLineEdit class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineEdit : public MessageComposer::ComposerLineEdit
{
    Q_OBJECT
public:
    explicit RecipientLineEdit(QWidget *parent);

Q_SIGNALS:
    void deleteMe();
    void leftPressed();
    void rightPressed();

protected:
    void keyPressEvent(QKeyEvent *ev) override;
};

/**
 * @brief The RecipientLineNG class
 */
class MESSAGECOMPOSER_EXPORT RecipientLineNG : public KPIM::MultiplyingLine
{
    Q_OBJECT
public:
    explicit RecipientLineNG(QWidget *parent);
    ~RecipientLineNG() override = default;

    void activate() override;
    [[nodiscard]] bool isActive() const override;

    [[nodiscard]] bool isEmpty() const override;
    void clear() override;
    [[nodiscard]] bool canDeleteLineEdit() const override;

    [[nodiscard]] bool isModified() const override;
    void clearModified() override;

    [[nodiscard]] KPIM::MultiplyingLineData::Ptr data() const override;
    void setData(const KPIM::MultiplyingLineData::Ptr &data) override;

    void fixTabOrder(QWidget *previous) override;
    [[nodiscard]] QWidget *tabOut() const override;

    void setCompletionMode(KCompletion::CompletionMode mode) override;

    [[nodiscard]] int setColumnWidth(int w) override;

    // recipient specific methods
    [[nodiscard]] int recipientsCount() const;

    void setRecipientType(Recipient::Type);
    [[nodiscard]] Recipient::Type recipientType() const;
    [[nodiscard]] QSharedPointer<Recipient> recipient() const;

    void setIcon(const QIcon &icon, const QString &tooltip = QString());

    /**
     * Sets the config file used for storing recent addresses.
     */
    void setRecentAddressConfig(KConfig *config);

    void setEnableIndexSearch(bool enableIndexSearch);
    [[nodiscard]] bool enableIndexSearch() const;

    void setEnableAkonadiSearch(bool enableAkonadiSearch);
    [[nodiscard]] bool enableAkonadiSearch() const;

    [[nodiscard]] QString rawData() const;

Q_SIGNALS:
    void typeModified(RecipientLineNG *);
    void addRecipient(RecipientLineNG *, const QString &);
    void countChanged();
    void iconClicked();
    void activeChanged();

protected Q_SLOTS:
    void slotEditingFinished();
    void slotTypeModified();
    void analyzeLine(const QString &);

protected:
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
