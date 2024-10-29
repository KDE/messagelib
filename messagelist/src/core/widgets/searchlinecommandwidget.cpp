/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "searchlinecommandwidget.h"

using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QWidget{parent}
{
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

#include "moc_searchlinecommandwidget.cpp"
