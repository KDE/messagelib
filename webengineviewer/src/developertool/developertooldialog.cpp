/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "developertooldialog.h"
#include "developertoolwidget.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QVBoxLayout>

namespace
{
static const char myDeveloperToolDialogConfigGroupName[] = "DeveloperToolDialog";
}

using namespace WebEngineViewer;
DeveloperToolDialog::DeveloperToolDialog(QWidget *parent)
    : QDialog(parent)
    , mDeveloperToolWidget(new DeveloperToolWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    mDeveloperToolWidget->setObjectName(QStringLiteral("mDeveloperToolWidget"));
    mainLayout->addWidget(mDeveloperToolWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
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
    KConfigGroup group(KSharedConfig::openStateConfig(), myDeveloperToolDialogConfigGroupName);
    const QSize sizeDialog = group.readEntry("Size", QSize(800, 600));
    if (sizeDialog.isValid()) {
        resize(sizeDialog);
    }
}

void DeveloperToolDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myDeveloperToolDialogConfigGroupName);
    group.writeEntry("Size", size());
}
