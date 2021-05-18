/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidget.h"
#include <QListWidget>
#include <QVBoxLayout>
using namespace MessageList::Core;
ConfigureFiltersWidget::ConfigureFiltersWidget(QWidget *parent)
    : QWidget(parent)
    , mListFiltersWidget(new QListWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mListFiltersWidget->setObjectName(QStringLiteral("mListFiltersWidget"));
    mainLayout->addWidget(mListFiltersWidget);
}

ConfigureFiltersWidget::~ConfigureFiltersWidget()
{
}
