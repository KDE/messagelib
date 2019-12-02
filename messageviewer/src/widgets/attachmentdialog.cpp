/*
  This file is part of KMail, the KDE mail client.
  Copyright (c) 2009 Martin Koller <kollix@aon.at>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "attachmentdialog.h"

#include <QDialog>
#include <KMessageBox>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QDialogButtonBox>
#include <QPushButton>
#include <KGuiItem>
using namespace MessageViewer;

//---------------------------------------------------------------------

AttachmentDialog::AttachmentDialog(QWidget *parent, const QString &filenameText, const KService::Ptr &offer, const QString &dontAskAgainName)
    : dontAskName(dontAskAgainName)
{
    text = xi18nc("@info", "Open attachment <filename>%1</filename>?<nl/>"
                           "Note that opening an attachment may compromise "
                           "your system's security.",
                  filenameText);

    dialog = new QDialog(parent);
    dialog->setWindowTitle(i18n("Open Attachment?"));
    dialog->setObjectName(QStringLiteral("attachmentSaveOpen"));
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Cancel, dialog);
    QPushButton *user1Button = new QPushButton;
    mButtonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    dialog->connect(mButtonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    dialog->connect(mButtonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (offer) {
        QPushButton *user2Button = new QPushButton;
        mButtonBox->addButton(user2Button, QDialogButtonBox::ActionRole);
        user2Button->setText(i18n("&Open With '%1'", offer->name()));
        user2Button->setIcon(QIcon::fromTheme(offer->icon()));
        connect(user2Button, &QPushButton::clicked, this, &AttachmentDialog::openClicked);
    }

    QPushButton *user3Button = new QPushButton;
    mButtonBox->addButton(user3Button, QDialogButtonBox::ActionRole);

    KGuiItem::assign(user3Button, KStandardGuiItem::saveAs());
    user1Button->setText(i18n("&Open With..."));
    user1Button->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    user3Button->setDefault(true);

    connect(user3Button, &QPushButton::clicked, this, &AttachmentDialog::saveClicked);
    connect(user1Button, &QPushButton::clicked, this, &AttachmentDialog::openWithClicked);
}

//---------------------------------------------------------------------

int AttachmentDialog::exec()
{
    KConfigGroup cg(KSharedConfig::openConfig().data(), "Notification Messages");
    if (cg.hasKey(dontAskName)) {
        return cg.readEntry(dontAskName, 0);
    }

    bool again = false;
    const QDialogButtonBox::StandardButton ret = KMessageBox::createKMessageBox(dialog, mButtonBox,
                                                                                QMessageBox::Question, text,
                                                                                QStringList(),
                                                                                i18n(
                                                                                    "Do not ask again"), &again,
                                                                                nullptr);

    if (ret == QDialogButtonBox::Cancel) {
        return Cancel;
    } else {
        if (again) {
            KConfigGroup::WriteConfigFlags flags = KConfig::Persistent;
            KConfigGroup cg(KSharedConfig::openConfig().data(), "Notification Messages");
            cg.writeEntry(dontAskName, static_cast<int>(ret), flags);
            cg.sync();
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
