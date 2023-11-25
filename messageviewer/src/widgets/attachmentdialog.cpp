/*
This file is part of KMail, the KDE mail client.
SPDX-FileCopyrightText: 2009 Martin Koller <kollix@aon.at>

SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "attachmentdialog.h"

#include <KConfigGroup>
#include <KGuiItem>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
using namespace MessageViewer;

//---------------------------------------------------------------------

AttachmentDialog::AttachmentDialog(QWidget *parent, const QString &filenameText, const KService::Ptr &offer, const QString &dontAskAgainName)
    : dontAskName(dontAskAgainName)
    , dialog(new QDialog(parent))
{
    text = xi18nc("@info",
                  "Open attachment <filename>%1</filename>?<nl/>"
                  "Note that opening an attachment may compromise "
                  "your system's security.",
                  filenameText);

    dialog->setWindowTitle(i18nc("@title:window", "Open Attachment?"));
    dialog->setObjectName(QLatin1StringView("attachmentSaveOpen"));
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, dialog);

    if (offer) {
        auto user1Button = new QPushButton;
        mButtonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
        user1Button->setText(i18n("&Open With '%1'", offer->name()));
        user1Button->setIcon(QIcon::fromTheme(offer->icon()));
        connect(user1Button, &QPushButton::clicked, this, &AttachmentDialog::openClicked);
    }

    auto user2Button = new QPushButton;
    mButtonBox->addButton(user2Button, QDialogButtonBox::ActionRole);
    user2Button->setText(i18n("Open &With..."));
    user2Button->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    connect(user2Button, &QPushButton::clicked, this, &AttachmentDialog::openWithClicked);

    auto user3Button = new QPushButton;
    mButtonBox->addButton(user3Button, QDialogButtonBox::ActionRole);
    KGuiItem::assign(user3Button, KStandardGuiItem::saveAs());
    user3Button->setDefault(true);
    connect(user3Button, &QPushButton::clicked, this, &AttachmentDialog::saveClicked);
}

//---------------------------------------------------------------------

int AttachmentDialog::exec()
{
    KConfigGroup cg(KSharedConfig::openConfig().data(), QStringLiteral("Notification Messages"));
    if (cg.hasKey(dontAskName)) {
        return cg.readEntry(dontAskName, 0);
    }

    bool again = false;
    const QDialogButtonBox::StandardButton ret =
        KMessageBox::createKMessageBox(dialog, mButtonBox, QMessageBox::Question, text, QStringList(), i18n("Do not ask again"), &again, KMessageBox::Option());

    if (ret == QDialogButtonBox::Cancel) {
        return Cancel;
    } else {
        if (again) {
            KConfigGroup::WriteConfigFlags flags = KConfig::Persistent;
            KConfigGroup configGroup(KSharedConfig::openConfig().data(), QStringLiteral("Notification Messages"));
            configGroup.writeEntry(dontAskName, static_cast<int>(ret), flags);
            configGroup.sync();
        }
        return ret;
    }
}

//---------------------------------------------------------------------

void AttachmentDialog::saveClicked()
{
    dialog->done(Save);
}

//---------------------------------------------------------------------

void AttachmentDialog::openClicked()
{
    dialog->done(Open);
}

//---------------------------------------------------------------------

void AttachmentDialog::openWithClicked()
{
    dialog->done(OpenWith);
}

//---------------------------------------------------------------------

#include "moc_attachmentdialog.cpp"
