/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

#include "dkimmanagerulesdialog.h"
#include "dkimmanageruleswidget.h"
#include "dkimmanagerrules.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KMessageBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPushButton>

namespace {
static const char myConfigGroupName[] = "DKIMManageRulesDialog";
}
using namespace MessageViewer;

DKIMManageRulesDialog::DKIMManageRulesDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Configure Rule"));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));

    mRulesWidget = new DKIMManageRulesWidget(this);
    mRulesWidget->setObjectName(QStringLiteral("mRulesWidget"));
    mainLayout->addWidget(mRulesWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    mainLayout->addWidget(buttonBox);

    QPushButton *addButton = new QPushButton(i18n("Add Rule..."), this);
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    connect(addButton, &QPushButton::clicked, mRulesWidget, &DKIMManageRulesWidget::addRule);

    QPushButton *importButton = new QPushButton(i18n("Import..."), this);
    buttonBox->addButton(importButton, QDialogButtonBox::ActionRole);
    connect(importButton, &QPushButton::clicked, this, &DKIMManageRulesDialog::slotImport);

    QPushButton *exportButton = new QPushButton(i18n("Export..."), this);
    buttonBox->addButton(exportButton, QDialogButtonBox::ActionRole);
    connect(exportButton, &QPushButton::clicked, this, &DKIMManageRulesDialog::slotExport);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DKIMManageRulesDialog::slotAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMManageRulesDialog::reject);
    mRulesWidget->loadSettings();
    readConfig();
}

DKIMManageRulesDialog::~DKIMManageRulesDialog()
{
    writeConfig();
}

void DKIMManageRulesDialog::slotAccepted()
{
    mRulesWidget->saveSettings();
    accept();
}

void DKIMManageRulesDialog::slotExport()
{
    if (MessageViewer::DKIMManagerRules::self()->isEmpty()) {
        KMessageBox::error(this, i18n("No rules found"), i18n("Export Rules"));
        return;
    }
    const QString fileName = QFileDialog::getSaveFileName(this, i18n("Export Rules"));
    if (!fileName.isEmpty()) {
        MessageViewer::DKIMManagerRules::self()->exportRules(fileName);
    }
}

void DKIMManageRulesDialog::slotImport()
{
    const QString fileName = QFileDialog::getOpenFileName(this, i18n("Import Rules"));
    if (!fileName.isEmpty()) {
        if (MessageViewer::DKIMManagerRules::self()->importRules(fileName) == 0) {
            KMessageBox::error(this, i18n("No rules imported."), i18n("Import Rules"));
        }
    }
}

void DKIMManageRulesDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    mRulesWidget->restoreHeaders(group.readEntry("Header", QByteArray()));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMManageRulesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), myConfigGroupName);
    group.writeEntry("Header", mRulesWidget->saveHeaders());
    group.writeEntry("Size", size());
    group.sync();
}
