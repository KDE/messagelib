/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchmessagebybuttons.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

using namespace MessageList::Core;
SearchMessageByButtons::SearchMessageByButtons(QWidget *parent)
    : QWidget{parent}
    , mMainLayout(new QHBoxLayout(this))
{
    mMainLayout->setObjectName(QStringLiteral("mainLayout"));
    mMainLayout->setContentsMargins({});
    mMainLayout->setSpacing(0);
    // TODO
}

SearchMessageByButtons::~SearchMessageByButtons() = default;

void SearchMessageByButtons::createButtons()
{
}
