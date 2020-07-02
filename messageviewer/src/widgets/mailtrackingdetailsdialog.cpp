/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
