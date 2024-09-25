/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidget.h"
#include "searchlinecommandflowlayout.h"
#include <KLocalizedString>
#include <QPushButton>
#include <QVBoxLayout>
using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QWidget{parent}
{
    auto flowLayout = new SearchLineCommandFlowLayout(this);
    flowLayout->setObjectName(QStringLiteral("flowLayout"));
    flowLayout->setContentsMargins({});
    flowLayout->setSpacing(0);
    fillWidgets();
    Q_ASSERT(!mButtonsList.isEmpty());
    QHashIterator<QString, QString> i(mButtonsList);
    while (i.hasNext()) {
        i.next();
        flowLayout->addWidget(createPushButton(i.value(), i.key()));
    }
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

QPushButton *SearchLineCommandWidget::createPushButton(const QString &i18nStr, const QString &commandStr)
{
    auto pushButton = new QPushButton(i18nStr, this);
    pushButton->setObjectName(commandStr);
    connect(pushButton, &QPushButton::clicked, this, [this, commandStr]() {
        Q_EMIT insertCommand(commandStr + QLatin1Char(' '));
    });
    return pushButton;
}

void SearchLineCommandWidget::fillWidgets()
{
    mButtonsList = {
        {QStringLiteral("from:"), i18n("From")},
        {QStringLiteral("to:"), i18n("To")},
        {QStringLiteral("cc:"), i18n("Cc")},
        {QStringLiteral("bcc:"), i18n("Bcc")},
        {QStringLiteral("has:attachment"), i18n("Has Attachment")},
        {QStringLiteral("is:read"), i18n("Read")},
        {QStringLiteral("is:unread"), i18n("Unread")},
        {QStringLiteral("is:important"), i18n("Important")},
        // TODO add more
    };
}

#include "moc_searchlinecommandwidget.cpp"