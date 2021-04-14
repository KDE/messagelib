/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 Daniel Molkentin <molkentin@kde.org>
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "vcardviewer.h"
#include "settings/messageviewersettings.h"
#include <Akonadi/Contact/GrantleeContactViewer>

#include <kcontacts/vcardconverter.h>
using KContacts::Addressee;
using KContacts::VCardConverter;

#include <Akonadi/Contact/ContactDefaultActions>
#include <KLocalizedString>

#include <Akonadi/Contact/AddContactJob>
#include <KConfigGroup>
#include <KGuiItem>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageViewer;
namespace
{
static const char myVCardViewerConfigGroupName[] = "VCardViewer";
}
VCardViewer::VCardViewer(QWidget *parent, const QByteArray &vCard)
    : QDialog(parent)
    , mContactViewer(new KAddressBookGrantlee::GrantleeContactViewer(this))
    , mUser2Button(new QPushButton)
    , mUser3Button(new QPushButton)
{
    setWindowTitle(i18nc("@title:window", "vCard Viewer"));
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto user1Button = new QPushButton;
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    buttonBox->addButton(mUser3Button, QDialogButtonBox::ActionRole);
    buttonBox->addButton(mUser2Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &VCardViewer::reject);
    setModal(false);
    buttonBox->button(QDialogButtonBox::Close)->setDefault(true);

    KGuiItem::assign(user1Button, KGuiItem(i18n("&Import")));
    KGuiItem::assign(mUser2Button, KGuiItem(i18n("&Next Card")));
    KGuiItem::assign(mUser3Button, KGuiItem(i18n("&Previous Card")));

    auto actions = new Akonadi::ContactDefaultActions(this);
    actions->connectToView(mContactViewer);

    mContactViewer->setForceDisableQRCode(true);
    mainLayout->addWidget(mContactViewer);
    mainLayout->addWidget(buttonBox);

    VCardConverter vcc;
    mAddresseeList = vcc.parseVCards(vCard);
    if (!mAddresseeList.empty()) {
        mContactViewer->setRawContact(mAddresseeList.at(0));
        if (mAddresseeList.size() <= 1) {
            mUser2Button->setVisible(false);
            mUser3Button->setVisible(false);
        } else {
            mUser3Button->setEnabled(false);
        }
        connect(user1Button, &QPushButton::clicked, this, &VCardViewer::slotUser1);
        connect(mUser2Button, &QPushButton::clicked, this, &VCardViewer::slotUser2);
        connect(mUser3Button, &QPushButton::clicked, this, &VCardViewer::slotUser3);
    } else {
        mContactViewer->setRawContact(KContacts::Addressee());
        user1Button->setEnabled(false);
        mUser2Button->setVisible(false);
        mUser3Button->setVisible(false);
    }
    readConfig();
}

VCardViewer::~VCardViewer()
{
    writeConfig();
}

void VCardViewer::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myVCardViewerConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(300, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void VCardViewer::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myVCardViewerConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}

void VCardViewer::slotUser1()
{
    const KContacts::Addressee contact = mAddresseeList.at(mAddresseeListIndex);

    auto job = new Akonadi::AddContactJob(contact, this, this);
    job->start();
}

void VCardViewer::slotUser2()
{
    // next vcard
    mContactViewer->setRawContact(mAddresseeList.at(++mAddresseeListIndex));
    if ((mAddresseeListIndex + 1) == (mAddresseeList.count())) {
        mUser2Button->setEnabled(false);
    }
    mUser3Button->setEnabled(true);
}

void VCardViewer::slotUser3()
{
    // previous vcard
    mContactViewer->setRawContact(mAddresseeList.at(--mAddresseeListIndex));
    if (mAddresseeListIndex == 0) {
        mUser3Button->setEnabled(false);
    }
    mUser2Button->setEnabled(true);
}
