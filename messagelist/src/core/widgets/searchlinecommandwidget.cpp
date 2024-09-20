/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidget.h"
#include <KLocalizedString>
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

void SearchLineCommandWidget::fillWidgets()
{
    mButtonsList = {
        {QStringLiteral("from:"), i18n("From")},
        {QStringLiteral("to:"), i18n("To")},
        {QStringLiteral("cc:"), i18n("Cc")},
        {QStringLiteral("bcc:"), i18n("Bcc")},
        // TODO add more
    };
}

#include "moc_searchlinecommandwidget.cpp"
