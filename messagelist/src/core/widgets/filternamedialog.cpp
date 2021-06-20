/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamedialog.h"
#include "filternamewidget.h"
#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageList::Core;
FilterNameDialog::FilterNameDialog(QWidget *parent)
    : QDialog(parent)
    , mFilterNameWidget(new FilterNameWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    // TODO allow to change icon ?

    mFilterNameWidget->setObjectName(QStringLiteral("mFilterNameWidget"));
    mainLayout->addWidget(mFilterNameWidget);

    auto button = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(button);
    connect(button, &QDialogButtonBox::accepted, this, &FilterNameDialog::accept);
    connect(button, &QDialogButtonBox::rejected, this, &FilterNameDialog::reject);
    auto okButton = button->button(QDialogButtonBox::Ok);
    okButton->setEnabled(false);
    connect(mFilterNameWidget, &FilterNameWidget::updateOkButton, okButton, &QPushButton::setEnabled);
}

FilterNameDialog::~FilterNameDialog()
{
}

QString FilterNameDialog::filterName() const
{
    return mFilterNameWidget->filterName();
}

QString FilterNameDialog::iconName() const
{
    return mFilterNameWidget->iconName();
}

void FilterNameDialog::setExistingFilterNames(const QStringList &lst)
{
    mFilterNameWidget->setExistingFilterNames(lst);
}
