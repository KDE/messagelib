/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchlinecommandwidget.h"
#include "searchlinecommandbuttonswidget.h"
#include <QLabel>
#include <QVBoxLayout>

using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QWidget{parent}
    , mSearchLineCommandButtonsWidget(new SearchLineCommandButtonsWidget(this))
    , mLabel(new QLabel(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mSearchLineCommandButtonsWidget->setObjectName(QStringLiteral("mSearchLineCommandButtonsWidget"));
    mainLayout->addWidget(mSearchLineCommandButtonsWidget);

    mLabel->setObjectName(QStringLiteral("mLabel"));
    mainLayout->addWidget(mLabel);
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

#include "moc_searchlinecommandwidget.cpp"
