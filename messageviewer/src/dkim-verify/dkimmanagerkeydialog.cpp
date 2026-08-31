/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "dkimmanagerkeydialog.h"
#include "dkimmanagerkeywidget.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QVBoxLayout>

using namespace Qt::Literals::StringLiterals;

namespace
{
static const char myDKIMManagerKeyDialogConfigGroupName[] = "DKIMManagerKeyDialog";
}

using namespace MessageViewer;
DKIMManagerKeyDialog::DKIMManagerKeyDialog(QWidget *parent)
    : QDialog(parent)
    , mManagerWidget(new DKIMManagerKeyWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainlayout"_L1);

    mManagerWidget->setObjectName("managerWidget"_L1);
    mainLayout->addWidget(mManagerWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName("buttonbox"_L1);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMManagerKeyDialog::reject);
    mainLayout->addWidget(buttonBox);
    loadKeys();
    readConfig();
}

DKIMManagerKeyDialog::~DKIMManagerKeyDialog()
{
    writeConfig();
}

void DKIMManagerKeyDialog::readConfig()
{
    const KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMManagerKeyDialogConfigGroupName));
    mManagerWidget->restoreHeaders(group.readEntry("Header", QByteArray()));
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMManagerKeyDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMManagerKeyDialogConfigGroupName));
    group.writeEntry("Header", mManagerWidget->saveHeaders());
    group.writeEntry("Size", size());
    group.sync();
}

void DKIMManagerKeyDialog::loadKeys()
{
    mManagerWidget->loadKeys();
}

void DKIMManagerKeyDialog::slotAccept()
{
    mManagerWidget->saveKeys();
}

#include "moc_dkimmanagerkeydialog.cpp"
