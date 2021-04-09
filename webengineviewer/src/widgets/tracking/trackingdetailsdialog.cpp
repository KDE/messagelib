/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingdetailsdialog.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPIMTextEdit/RichTextEditorWidget>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace WebEngineViewer;
namespace
{
static const char myMailTrackingDetailsDialogConfigGroupName[] = "MailTrackingDetailsDialog";
}
TrackingDetailsDialog::TrackingDetailsDialog(QWidget *parent)
    : QDialog(parent)
    , mDetails(new KPIMTextEdit::RichTextEditorWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TrackingDetailsDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &TrackingDetailsDialog::close);

    mDetails->setObjectName(QStringLiteral("detail"));
    mainLayout->addWidget(mDetails);
    mainLayout->addWidget(buttonBox);
    mDetails->setReadOnly(true);
    readConfig();
}

TrackingDetailsDialog::~TrackingDetailsDialog()
{
    writeConfig();
}

void TrackingDetailsDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myMailTrackingDetailsDialogConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void TrackingDetailsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myMailTrackingDetailsDialogConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}

void TrackingDetailsDialog::setDetails(const QString &details)
{
    mDetails->setHtml(details);
}
