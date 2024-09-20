/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidget.h"
#include <QVBoxLayout>
using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QWidget{parent}
{
    auto vbox = new QVBoxLayout(this);
    vbox->setContentsMargins({});
    vbox->setSpacing(0);
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

#include "moc_searchlinecommandwidget.cpp"
