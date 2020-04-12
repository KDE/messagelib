/*
   Copyright (C) 2020 Laurent Montel <montel@kde.org>

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
#include "developertooldialog.h"
#include "developertoolwidget.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>

namespace {
static const char myConfigGroupName[] = "DeveloperToolDialog";
}

using namespace MessageViewer;
DeveloperToolDialog::DeveloperToolDialog(QWidget *parent)
    : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
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
