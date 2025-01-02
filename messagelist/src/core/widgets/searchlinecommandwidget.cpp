/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

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
    mLabel->setWordWrap(true);

    connect(mSearchLineCommandButtonsWidget, &SearchLineCommandButtonsWidget::insertCommand, this, &SearchLineCommandWidget::insertCommand);
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

void SearchLineCommandWidget::setLabel(const QString &str)
{
    mLabel->setText(str);
}

#include "moc_searchlinecommandwidget.cpp"
