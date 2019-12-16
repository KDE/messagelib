/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mailtrackingdetailsdialog.h"
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <KSharedConfig>
#include <KPIMTextEdit/RichTextEditorWidget>

using namespace MessageViewer;
MailTrackingDetailsDialog::MailTrackingDetailsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailTrackingDetailsDialog::reject);
    connect(buttonBox->button(
                QDialogButtonBox::Close), &QPushButton::clicked, this,
            &MailTrackingDetailsDialog::close);

    mDetails = new KPIMTextEdit::RichTextEditorWidget(this);
    mDetails->setObjectName(QStringLiteral("detail"));
    mainLayout->addWidget(mDetails);
    mainLayout->addWidget(buttonBox);
    mDetails->setReadOnly(true);
    readConfig();
}

MailTrackingDetailsDialog::~MailTrackingDetailsDialog()
{
    writeConfig();
}

void MailTrackingDetailsDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "MailTrackingDetailsDialog");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void MailTrackingDetailsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "MailTrackingDetailsDialog");
    group.writeEntry("Size", size());
    group.sync();
}

void MailTrackingDetailsDialog::setDetails(const QString &details)
{
    mDetails->setHtml(details);
}
