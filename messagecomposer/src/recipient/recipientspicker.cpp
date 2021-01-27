/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "recipientspicker.h"
#include "settings/messagecomposersettings.h"

#include <Akonadi/Contact/EmailAddressSelectionWidget>
#include <Akonadi/Contact/RecipientsPickerWidget>
#include <PimCommonAkonadi/LdapSearchDialog>
#include <kcontacts/contactgroup.h>

#include "messagecomposer_debug.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <QLineEdit>
#include <QPushButton>

#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QTreeView>
#include <QVBoxLayout>

using namespace MessageComposer;

RecipientsPicker::RecipientsPicker(QWidget *parent)
    : QDialog(parent)
{
    setObjectName(QStringLiteral("RecipientsPicker"));
    setWindowTitle(i18nc("@title:window", "Select Recipient"));

    auto mainLayout = new QVBoxLayout(this);

    mView = new Akonadi::RecipientsPickerWidget(true, nullptr, this);
    mainLayout->addWidget(mView);
    mainLayout->setStretchFactor(mView, 1);

    connect(mView->view()->selectionModel(), &QItemSelectionModel::selectionChanged, this, &RecipientsPicker::slotSelectionChanged);
    connect(mView->view(), &QAbstractItemView::doubleClicked, this, &RecipientsPicker::slotPicked);

    auto searchLDAPButton = new QPushButton(i18n("Search &Directory Service"), this);
    connect(searchLDAPButton, &QPushButton::clicked, this, &RecipientsPicker::slotSearchLDAP);
    mainLayout->addWidget(searchLDAPButton);

    KConfig config(QStringLiteral("kabldaprc"));
    KConfigGroup group = config.group("LDAP");
    int numHosts = group.readEntry("NumSelectedHosts", 0);
    if (!numHosts) {
        searchLDAPButton->setVisible(false);
    }

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    mUser1Button = new QPushButton;
    buttonBox->addButton(mUser1Button, QDialogButtonBox::ActionRole);
    mUser2Button = new QPushButton;
    buttonBox->addButton(mUser2Button, QDialogButtonBox::ActionRole);
    mUser3Button = new QPushButton;
    buttonBox->addButton(mUser3Button, QDialogButtonBox::ActionRole);
    mUser4Button = new QPushButton;
    buttonBox->addButton(mUser4Button, QDialogButtonBox::ActionRole);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &RecipientsPicker::reject);
    mainLayout->addWidget(buttonBox);
    mUser4Button->setText(i18n("Add as &Reply-To"));
    mUser3Button->setText(i18n("Add as &To"));
    mUser2Button->setText(i18n("Add as CC"));
    mUser1Button->setText(i18n("Add as &BCC"));
    connect(mUser1Button, &QPushButton::clicked, this, &RecipientsPicker::slotBccClicked);
    connect(mUser2Button, &QPushButton::clicked, this, &RecipientsPicker::slotCcClicked);
    connect(mUser3Button, &QPushButton::clicked, this, &RecipientsPicker::slotToClicked);
    connect(mUser4Button, &QPushButton::clicked, this, &RecipientsPicker::slotReplyToClicked);

    mView->emailAddressSelectionWidget()->searchLineEdit()->setFocus();

    readConfig();

    slotSelectionChanged();
}

RecipientsPicker::~RecipientsPicker()
{
    writeConfig();
}

void RecipientsPicker::slotSelectionChanged()
{
    const bool hasSelection = !mView->emailAddressSelectionWidget()->selectedAddresses().isEmpty();
    mUser1Button->setEnabled(hasSelection);
    mUser2Button->setEnabled(hasSelection);
    mUser3Button->setEnabled(hasSelection);
    mUser4Button->setEnabled(hasSelection);
}

void RecipientsPicker::setRecipients(const Recipient::List &)
{
    mView->view()->selectionModel()->clear();
}

void RecipientsPicker::setDefaultType(Recipient::Type type)
{
    mDefaultType = type;
    mUser1Button->setDefault(type == Recipient::Bcc);
    mUser2Button->setDefault(type == Recipient::Cc);
    mUser3Button->setDefault(type == Recipient::To);
    mUser4Button->setDefault(type == Recipient::ReplyTo);
}

void RecipientsPicker::slotToClicked()
{
    pick(Recipient::To);
}

void RecipientsPicker::slotReplyToClicked()
{
    pick(Recipient::ReplyTo);
}

void RecipientsPicker::slotCcClicked()
{
    pick(Recipient::Cc);
}

void RecipientsPicker::slotBccClicked()
{
    pick(Recipient::Bcc);
}

void RecipientsPicker::slotPicked()
{
    pick(mDefaultType);
}

void RecipientsPicker::pick(Recipient::Type type)
{
    qCDebug(MESSAGECOMPOSER_LOG) << int(type);

    const Akonadi::EmailAddressSelection::List selections = mView->emailAddressSelectionWidget()->selectedAddresses();

    const int count = selections.count();
    if (count == 0) {
        return;
    }

    if (count > MessageComposerSettings::self()->maximumRecipients()) {
        KMessageBox::sorry(this,
                           i18np("You selected 1 recipient. The maximum supported number of "
                                 "recipients is %2. Please adapt the selection.",
                                 "You selected %1 recipients. The maximum supported number of "
                                 "recipients is %2. Please adapt the selection.",
                                 count,
                                 MessageComposerSettings::self()->maximumRecipients()));
        return;
    }

    bool tooManyAddress = false;
    for (const Akonadi::EmailAddressSelection &selection : selections) {
        Recipient recipient;
        recipient.setType(type);
        recipient.setEmail(selection.quotedEmail());

        Q_EMIT pickedRecipient(recipient, tooManyAddress);
        if (tooManyAddress) {
            break;
        }
    }
}

void RecipientsPicker::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }

    QDialog::keyPressEvent(event);
}

void RecipientsPicker::readConfig()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
    KConfigGroup group(cfg, "RecipientsPicker");
    QSize size = group.readEntry("Size", QSize());
    if (!size.isEmpty()) {
        resize(size);
    }
}

void RecipientsPicker::writeConfig()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig();
    KConfigGroup group(cfg, "RecipientsPicker");
    group.writeEntry("Size", size());
}

void RecipientsPicker::slotSearchLDAP()
{
    if (!mLdapSearchDialog) {
        mLdapSearchDialog = new PimCommon::LdapSearchDialog(this);
        connect(mLdapSearchDialog, &PimCommon::LdapSearchDialog::contactsAdded, this, &RecipientsPicker::ldapSearchResult);
    }

    mLdapSearchDialog->setSearchText(mView->emailAddressSelectionWidget()->searchLineEdit()->text());
    mLdapSearchDialog->show();
}

void RecipientsPicker::ldapSearchResult()
{
    const KContacts::Addressee::List contacts = mLdapSearchDialog->selectedContacts();
    for (const KContacts::Addressee &contact : contacts) {
        bool tooManyAddress = false;
        Q_EMIT pickedRecipient(Recipient(contact.fullEmail(), Recipient::Undefined), tooManyAddress);
        if (tooManyAddress) {
            break;
        }
    }
}
