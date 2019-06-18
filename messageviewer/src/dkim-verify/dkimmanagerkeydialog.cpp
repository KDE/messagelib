/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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
#include "dkimmanagerkeydialog.h"
#include "dkimmanagerkeywidget.h"


#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <KSharedConfig>

using namespace MessageViewer;
DKIMManagerKeyDialog::DKIMManagerKeyDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));

    mManagerWidget = new DKIMManagerKeyWidget(this);
    mManagerWidget->setObjectName(QStringLiteral("managerWidget"));
    mainLayout->addWidget(mManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QStringLiteral("buttonbox"));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DKIMManagerKeyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMManagerKeyDialog::reject);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

DKIMManagerKeyDialog::~DKIMManagerKeyDialog()
{
    writeConfig();
}

void DKIMManagerKeyDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "DKIMManagerKeyDialog");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMManagerKeyDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), "DKIMManagerKeyDialog");
    group.writeEntry("Size", size());
    group.sync();
}

void DKIMManagerKeyDialog::loadKeys()
{
    mManagerWidget->loadKeys();
}

void DKIMManagerKeyDialog::saveKeys()
{
    mManagerWidget->saveKeys();
}
