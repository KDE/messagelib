/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <MessageComposer/Recipient>

#include <KContacts/Addressee>
#include <QDialog>

class QPushButton;
class QLabel;
namespace PimCommon
{
class LdapSearchDialog;
}

namespace Akonadi
{
class RecipientsPickerWidget;
}

namespace MessageComposer
{
class RecipientsPicker : public QDialog
{
    Q_OBJECT

public:
    explicit RecipientsPicker(QWidget *parent);
    ~RecipientsPicker() override;

    void setRecipients(const Recipient::List &);

    void setDefaultType(Recipient::Type);

Q_SIGNALS:
    void pickedRecipient(const Recipient &, bool &);

protected:
    void readConfig();
    void writeConfig();

    void pick(Recipient::Type);

    void keyPressEvent(QKeyEvent *) override;

protected Q_SLOTS:
    void slotToClicked();
    void slotCcClicked();
    void slotBccClicked();
    void slotReplyToClicked();
    void slotPicked();
    void slotSearchLDAP();
    void ldapSearchResult();
    void slotSelectionChanged();

private:
    void updateLabel(int nbSelected);
    Akonadi::RecipientsPickerWidget *const mView;

    PimCommon::LdapSearchDialog *mLdapSearchDialog = nullptr;

    Recipient::Type mDefaultType;
    QPushButton *const mUser1Button;
    QPushButton *const mUser2Button;
    QPushButton *const mUser3Button;
    QPushButton *const mUser4Button;
    QLabel *const mSelectedLabel;
};
}
