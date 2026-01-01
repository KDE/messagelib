/*
   SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidgetgui.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <core/widgets/searchlinecommand.h>
#include <core/widgets/searchlinecommandbuttonswidget.h>
using namespace Qt::Literals::StringLiterals;

SearchLineCommandWidgetGui::SearchLineCommandWidgetGui(QWidget *parent)
    : QWidget{parent}
    , mLineEdit(new QLineEdit(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins({});

    mainLayout->addWidget(mLineEdit);
    MessageList::Core::SearchLineCommandButtonsWidget *w = new MessageList::Core::SearchLineCommandButtonsWidget(this);
    mainLayout->addWidget(w);
    mainLayout->addStretch(1);
    auto label = new QLabel(this);
    mainLayout->addWidget(label);
    mLineEdit->setClearButtonEnabled(true);
    label->setWordWrap(true);
    connect(w, &MessageList::Core::SearchLineCommandButtonsWidget::insertCommand, this, [this](const QString &commandStr) {
        if (!mLineEdit->text().isEmpty() && mLineEdit->text().back() != u' ') {
            mLineEdit->insert(u" "_s);
        }
        mLineEdit->insert(commandStr);
    });
    connect(mLineEdit, &QLineEdit::textChanged, this, [label](const QString &str) {
        MessageList::Core::SearchLineCommand c;
        c.parseSearchLineCommand(str);
        label->setText(c.generateCommadLineStr());
    });
}

SearchLineCommandWidgetGui::~SearchLineCommandWidgetGui() = default;

#include "moc_searchlinecommandwidgetgui.cpp"
