/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "developertooldialog.h"
#include "developertoolwidget.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <KSharedConfig>

namespace {
static const char myConfigGroupName[] = "DeveloperToolDialog";
}

using namespace MessageViewer;
DeveloperToolDialog::DeveloperToolDialog(QWidget *parent)
    : QDialog(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mDeveloperToolWidget = new DeveloperToolWidget(this);
    mDeveloperToolWidget->setObjectName(QStringLiteral("mDeveloperToolWidget"));
    mainLayout->addWidget(mDeveloperToolWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeveloperToolDialog::reject);
    mainLayout->addWidget(buttonBox);
    readConfig();
}

DeveloperToolDialog::~DeveloperToolDialog()
{
    writeConfig();
}

QWebEnginePage *DeveloperToolDialog::enginePage() const
{
    return mDeveloperToolWidget->enginePage();
}

void DeveloperToolDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    const QSize sizeDialog = group.readEntry("Size", QSize(800, 600));
    if (sizeDialog.isValid()) {
        resize(sizeDialog);
    }
}

void DeveloperToolDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    group.writeEntry("Size", size());
}
