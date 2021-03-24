/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentdialog.h"
#include "remotecontentwidget.h"
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
using namespace MessageViewer;
RemoteContentDialog::RemoteContentDialog(QWidget *parent)
    : QDialog(parent)
    , mRemoveContentWidget(new RemoteContentWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mRemoveContentWidget->setObjectName(QStringLiteral("mRemoveContentWidget"));
    mainLayout->addWidget(mRemoveContentWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &RemoteContentDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RemoteContentDialog::reject);
    mainLayout->addWidget(buttonBox);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);
    connect(mRemoveContentWidget, &RemoteContentWidget::updateOkButton, this, [okButton](bool enabled) {
        okButton->setEnabled(enabled);
    });
}

RemoteContentDialog::~RemoteContentDialog()
{
}

RemoteContentInfo RemoteContentDialog::info() const
{
    return mRemoveContentWidget->info();
}

void RemoteContentDialog::setInfo(const RemoteContentInfo &info)
{
    mRemoveContentWidget->setInfo(info);
}
