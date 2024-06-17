/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerulesdialog.h"
#include "dkimmanagerrules.h"
#include "dkimmanageruleswidget.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
static const char myDKIMManageRulesDialogGroupName[] = "DKIMManageRulesDialog";
}
using namespace MessageViewer;

DKIMManageRulesDialog::DKIMManageRulesDialog(QWidget *parent)
    : QDialog(parent)
    , mRulesWidget(new DKIMManageRulesWidget(this))
    , mExportButton(new QPushButton(i18nc("@action:button", "Export..."), this))
{
    setWindowTitle(i18nc("@title:window", "Configure Rule"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));

    mRulesWidget->setObjectName(QLatin1StringView("mRulesWidget"));
    mainLayout->addWidget(mRulesWidget);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->setObjectName(QLatin1StringView("buttonBox"));
    mainLayout->addWidget(buttonBox);

    auto addButton = new QPushButton(i18nc("@action:button", "Add Rule..."), this);
    addButton->setObjectName(QLatin1StringView("addButton"));
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    connect(addButton, &QPushButton::clicked, mRulesWidget, &DKIMManageRulesWidget::addRule);

    auto importButton = new QPushButton(i18nc("@action:button", "Import..."), this);
    importButton->setObjectName(QLatin1StringView("importButton"));
    buttonBox->addButton(importButton, QDialogButtonBox::ActionRole);
    connect(importButton, &QPushButton::clicked, this, &DKIMManageRulesDialog::slotImport);

    mExportButton->setObjectName(QLatin1StringView("mExportButton"));
    buttonBox->addButton(mExportButton, QDialogButtonBox::ActionRole);
    connect(mExportButton, &QPushButton::clicked, this, &DKIMManageRulesDialog::slotExport);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DKIMManageRulesDialog::slotAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DKIMManageRulesDialog::reject);
    connect(mRulesWidget, &DKIMManageRulesWidget::updateExportButton, mExportButton, &QPushButton::setEnabled);
    mExportButton->setEnabled(false);
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
    const auto rules = mRulesWidget->rules();
    if (rules.isEmpty()) {
        KMessageBox::error(this, i18n("No rules found"), i18nc("@title:window", "Export Rules"));
        return;
    }
    const QString fileName = QFileDialog::getSaveFileName(this, i18n("Export Rules"));
    if (!fileName.isEmpty()) {
        MessageViewer::DKIMManagerRules::self()->exportRules(fileName, rules);
    }
}

void DKIMManageRulesDialog::slotImport()
{
    const QString fileName = QFileDialog::getOpenFileName(this, i18n("Import Rules"));
    if (!fileName.isEmpty()) {
        if (MessageViewer::DKIMManagerRules::self()->importRules(fileName) == 0) {
            KMessageBox::error(this, i18n("No rules imported."), i18nc("@title:window", "Import Rules"));
        } else {
            mRulesWidget->updateRules();
        }
    }
}

void DKIMManageRulesDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMManageRulesDialogGroupName));
    const QSize size = group.readEntry("Size", QSize(600, 400));
    mRulesWidget->restoreHeaders(group.readEntry("Header", QByteArray()));
    if (size.isValid()) {
        resize(size);
    }
}

void DKIMManageRulesDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myDKIMManageRulesDialogGroupName));
    group.writeEntry("Header", mRulesWidget->saveHeaders());
    group.writeEntry("Size", size());
    group.sync();
}

#include "moc_dkimmanagerulesdialog.cpp"
