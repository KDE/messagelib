/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "configurefiltersdialog.h"
#include "configurefilterswidget.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>

using namespace MessageList::Core;
ConfigureFiltersDialog::ConfigureFiltersDialog(QWidget *parent)
    : QDialog(parent)
    , mConfigureFiltersWidget(new ConfigureFiltersWidget(this))
{
}

ConfigureFiltersDialog::~ConfigureFiltersDialog()
{
}
