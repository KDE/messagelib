/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mailtrackingdetailsdialog.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPIMTextEdit/RichTextEditorWidget>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageViewer;
namespace
{
static const char myMailTrackingDetailsDialogConfigGroupName[] = "MailTrackingDetailsDialog";
}
MailTrackingDetailsDialog::MailTrackingDetailsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MailTrackingDetailsDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &MailTrackingDetailsDialog::close);

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
    KConfigGroup group(KSharedConfig::openConfig(), myMailTrackingDetailsDialogConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void MailTrackingDetailsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myMailTrackingDetailsDialogConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}

void MailTrackingDetailsDialog::setDetails(const QString &details)
{
    mDetails->setHtml(details);
}
