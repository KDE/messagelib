/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "configurefiltersdialog.h"
#include "configurefilterswidget.h"
#include <KConfigGroup>
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
    readConfig();
}

ConfigureFiltersDialog::~ConfigureFiltersDialog()
{
    writeConfig();
}

void ConfigureFiltersDialog::readConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myConfigureFiltersDialogConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void ConfigureFiltersDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myConfigureFiltersDialogConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}
