/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    This file was part of KMail.
    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "recipientspicker.h"
#include "settings/messagecomposersettings.h"

#include <Akonadi/EmailAddressSelectionWidget>
#include <Akonadi/RecipientsPickerWidget>
#include <KContacts/ContactGroup>
#include <PimCommonAkonadi/LdapSearchDialog>

#include "messagecomposer_debug.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KWindowConfig>
#include <QLineEdit>
#include <QPushButton>
#include <QWindow>

#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QLabel>
#include <QTreeView>
#include <QVBoxLayout>

using namespace MessageComposer;
namespace
{
static const char myRecipientsPickerConfigGroupName[] = "RecipientsPicker";
}
RecipientsPicker::RecipientsPicker(QWidget *parent)
    : QDialog(parent)
    , mView(new Akonadi::RecipientsPickerWidget(true, nullptr, this))
    , mUser1Button(new QPushButton(this))
    , mUser2Button(new QPushButton(this))
    , mUser3Button(new QPushButton(this))
    , mUser4Button(new QPushButton(this))
    , mSelectedLabel(new QLabel(this))
{
    // TODO add accountactivitiesabstract
    setObjectName(QLatin1StringView("RecipientsPicker"));
    setWindowTitle(i18nc("@title:window", "Select Recipient"));

    auto mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(mView);
    mainLayout->setStretchFactor(mView, 1);

    connect(mView->view()->selectionModel(), &QItemSelectionModel::selectionChanged, this, &RecipientsPicker::slotSelectionChanged);
    connect(mView->view(), &QAbstractItemView::doubleClicked, this, &RecipientsPicker::slotPicked);

    mainLayout->addWidget(mSelectedLabel);

    auto searchLDAPButton = new QPushButton(i18nc("@action:button", "Search &Directory Service"), this);
    connect(searchLDAPButton, &QPushButton::clicked, this, &RecipientsPicker::slotSearchLDAP);
    mainLayout->addWidget(searchLDAPButton);

    KConfig config(QStringLiteral("kabldaprc"));
    KConfigGroup group = config.group(QStringLiteral("LDAP"));
    int numHosts = group.readEntry("NumSelectedHosts", 0);
    if (!numHosts) {
        searchLDAPButton->setVisible(false);
    }

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->addButton(mUser1Button, QDialogButtonBox::ActionRole);
    buttonBox->addButton(mUser2Button, QDialogButtonBox::ActionRole);
    buttonBox->addButton(mUser3Button, QDialogButtonBox::ActionRole);
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

void RecipientsPicker::updateLabel(int nbSelected)
{
    if (nbSelected == 0) {
        mSelectedLabel->setText({});
    } else {
        mSelectedLabel->setText(i18np("%1 selected email", "%1 selected emails", nbSelected));
    }
}

void RecipientsPicker::slotSelectionChanged()
{
    const auto selectedItems{mView->emailAddressSelectionWidget()->selectedAddresses().count()};
    const bool hasSelection = (selectedItems != 0);
    mUser1Button->setEnabled(hasSelection);
    mUser2Button->setEnabled(hasSelection);
    mUser3Button->setEnabled(hasSelection);
    mUser4Button->setEnabled(hasSelection);
    updateLabel(selectedItems);
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
        KMessageBox::error(this,
                           i18np("You selected 1 recipient. The maximum supported number of "
                                 "recipients is %2.\nPlease adapt the selection.",
                                 "You selected %1 recipients. The maximum supported number of "
                                 "recipients is %2.\nPlease adapt the selection.",
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
            KMessageBox::error(this, i18n("You can not add more recipients."));
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
    create(); // ensure a window is created
    windowHandle()->resize(QSize(300, 200));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myRecipientsPickerConfigGroupName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void RecipientsPicker::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myRecipientsPickerConfigGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
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

#include "moc_recipientspicker.cpp"
