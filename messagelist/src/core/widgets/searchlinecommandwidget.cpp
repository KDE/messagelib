/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandwidget.h"
#include "searchlinecommandflowlayout.h"
#include <KLocalizedString>
#include <QEvent>
#include <QPushButton>
#include <QVBoxLayout>
using namespace MessageList::Core;
SearchLineCommandWidget::SearchLineCommandWidget(QWidget *parent)
    : QFrame{parent}
{
    auto flowLayout = new SearchLineCommandFlowLayout(this);
    flowLayout->setObjectName(QStringLiteral("flowLayout"));
    flowLayout->setContentsMargins({});
    flowLayout->setSpacing(0);
    flowLayout->setHorizontalSpacing(0);
    flowLayout->setVerticalSpacing(0);
    fillWidgets();
    Q_ASSERT(!mButtonsList.isEmpty());
    for (const auto &info : std::as_const(mButtonsList)) {
        flowLayout->addWidget(createPushButton(info.needSpace, info.i18n, info.identifier));
    }
}

SearchLineCommandWidget::~SearchLineCommandWidget() = default;

QPushButton *SearchLineCommandWidget::createPushButton(bool needSpace, const QString &i18nStr, const QString &commandStr)
{
    auto pushButton = new QPushButton(i18nStr, this);
    pushButton->setObjectName(commandStr);
    pushButton->setToolTip(commandStr);
    connect(pushButton, &QPushButton::clicked, this, [this, commandStr, needSpace]() {
        const QString str = commandStr + (needSpace ? QStringLiteral(" ") : QString());
        Q_EMIT insertCommand(str);
    });
    return pushButton;
}

void SearchLineCommandWidget::fillWidgets()
{
    mButtonsList = {
        {false, QStringLiteral("subject:"), i18nc("@action:button", "Subject")},
        {false, QStringLiteral("from:"), i18nc("@action:button", "From")},
        {false, QStringLiteral("to:"), i18nc("@action:button", "To")},
        {false, QStringLiteral("cc:"), i18nc("@action:button", "Cc")},
        {false, QStringLiteral("bcc:"), i18nc("@action:button", "Bcc")},
        {true, QStringLiteral("has:attachment"), i18nc("@action:button", "Has Attachment")},
        {true, QStringLiteral("has:invitation"), i18nc("@action:button", "Has Invitation")},
        {true, QStringLiteral("is:read"), i18nc("@action:button", "Read")},
        {true, QStringLiteral("is:unread"), i18nc("@action:button", "Unread")},
        {true, QStringLiteral("is:important"), i18nc("@action:button", "Important")},
        {true, QStringLiteral("is:ignored"), i18nc("@action:button", "Ignored")},
        {true, QStringLiteral("is:ham"), i18nc("@action:button", "Ham")},
        {true, QStringLiteral("is:spam"), i18nc("@action:button", "Spam")},
        {true, QStringLiteral("is:watched"), i18nc("@action:button", "Watched")},
        {true, QStringLiteral("is:replied"), i18nc("@action:button", "Replied")},
        {true, QStringLiteral("is:forwarded"), i18nc("@action:button", "Forwarded")},
#if 0 // Reactivate when we implemented it.
        {false, QStringLiteral("smaller:"), i18nc("@action:button", "Smaller")},
        {false, QStringLiteral("larger:"), i18nc("@action:button", "Larger")},
        {false, QStringLiteral("size:"), i18nc("@action:button", "Size")},
        {false, QStringLiteral("older_than:"), i18nc("@action:button", "Older")},
        {false, QStringLiteral("newer_than:"), i18nc("@action:button", "Newer")},
#endif
    };
}

#include "moc_searchlinecommandwidget.cpp"
