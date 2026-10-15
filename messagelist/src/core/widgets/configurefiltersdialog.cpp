/*
  SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "configurefiltersdialog.h"
#include "configurefilterswidget.h"
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <TextAddonsWidgets/LoadDialogSizeUtils>

using namespace Qt::Literals::StringLiterals;

using namespace MessageList::Core;
namespace
{
const char myConfigureFiltersDialogConfigGroupName[] = "ConfigureFiltersDialog";
}

ConfigureFiltersDialog::ConfigureFiltersDialog(QWidget *parent)
    : QDialog(parent)
    , mConfigureFiltersWidget(new ConfigureFiltersWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Configure Filter"));
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainLayout"_L1);

    mConfigureFiltersWidget->setObjectName("mConfigureFiltersWidget"_L1);
    mainLayout->addWidget(mConfigureFiltersWidget);

    auto button = new QDialogButtonBox(QDialogButtonBox::Close, this);
    mainLayout->addWidget(button);
    connect(button, &QDialogButtonBox::rejected, this, &ConfigureFiltersDialog::reject);
    readConfig();
}

ConfigureFiltersDialog::~ConfigureFiltersDialog() = default;

void ConfigureFiltersDialog::readConfig()
{
    TextAddonsWidgets::LoadDialogSizeUtils::manageDialogSize(this, QLatin1StringView(myConfigureFiltersDialogConfigGroupName), QSize(600, 400));
}

#include "moc_configurefiltersdialog.cpp"
