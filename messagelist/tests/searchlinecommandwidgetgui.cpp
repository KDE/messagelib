/*
   SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidgetgui.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <core/widgets/searchlinecommandwidget.h>

SearchLineCommandWidgetGui::SearchLineCommandWidgetGui(QWidget *parent)
    : QWidget{parent}
    , mLineEdit(new QLineEdit(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins({});

    mainLayout->addWidget(mLineEdit);
    MessageList::Core::SearchLineCommandWidget *w = new MessageList::Core::SearchLineCommandWidget(this);
    mainLayout->addWidget(w);
    mainLayout->addStretch(1);
    connect(w, &MessageList::Core::SearchLineCommandWidget::insertCommand, mLineEdit, &QLineEdit::insert);
}

SearchLineCommandWidgetGui::~SearchLineCommandWidgetGui() = default;
