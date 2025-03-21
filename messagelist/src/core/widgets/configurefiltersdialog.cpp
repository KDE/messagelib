/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "configurefiltersdialog.h"
#include "configurefilterswidget.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QDialogButtonBox>
#include <QVBoxLayout>

using namespace MessageList::Core;
namespace
{
static const char myConfigureFiltersDialogConfigGroupName[] = "ConfigureFiltersDialog";
}

ConfigureFiltersDialog::ConfigureFiltersDialog(QWidget *parent)
    : QDialog(parent)
    , mConfigureFiltersWidget(new ConfigureFiltersWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Configure Filter"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));

    mConfigureFiltersWidget->setObjectName(QLatin1StringView("mConfigureFiltersWidget"));
    mainLayout->addWidget(mConfigureFiltersWidget);

    auto button = new QDialogButtonBox(QDialogButtonBox::Close, this);
    mainLayout->addWidget(button);
    connect(button, &QDialogButtonBox::rejected, this, &ConfigureFiltersDialog::reject);
    readConfig();
}

ConfigureFiltersDialog::~ConfigureFiltersDialog()
{
    writeConfig();
}

void ConfigureFiltersDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myConfigureFiltersDialogConfigGroupName));
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void ConfigureFiltersDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myConfigureFiltersDialogConfigGroupName));
    group.writeEntry("Size", size());
    group.sync();
}

#include "moc_configurefiltersdialog.cpp"
