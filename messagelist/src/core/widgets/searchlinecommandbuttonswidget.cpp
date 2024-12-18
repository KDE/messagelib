/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandbuttonswidget.h"
#include "searchlinecommand.h"
#include "searchlinecommandflowlayout.h"
#include <KLocalizedString>
#include <QEvent>
#include <QPushButton>
#include <QVBoxLayout>
using namespace MessageList::Core;
SearchLineCommandButtonsWidget::SearchLineCommandButtonsWidget(QWidget *parent)
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

SearchLineCommandButtonsWidget::~SearchLineCommandButtonsWidget() = default;

QPushButton *SearchLineCommandButtonsWidget::createPushButton(bool needSpace, const QString &i18nStr, const QString &commandStr)
{
    auto pushButton = new QPushButton(i18nStr, this);
    pushButton->setObjectName(commandStr);
    pushButton->setToolTip(commandStr);
    connect(pushButton, &QPushButton::clicked, this, [this, commandStr, needSpace]() {
        const QString str = commandStr + (needSpace ? QStringLiteral(": ") : QString());
        Q_EMIT insertCommand(str);
    });
    return pushButton;
}

void SearchLineCommandButtonsWidget::fillWidgets()
{
    mButtonsList = {
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::Subject), i18nc("@action:button", "Subject")},
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::Body), i18nc("@action:button", "Body")},
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::From), i18nc("@action:button", "From")},
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::To), i18nc("@action:button", "To")},
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::Cc), i18nc("@action:button", "Cc")},
        {false, SearchLineCommand::searchLineTypeToString(SearchLineCommand::Bcc), i18nc("@action:button", "Bcc")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::HasAttachment), i18nc("@action:button", "Has Attachment")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::HasInvitation), i18nc("@action:button", "Has Invitation")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsRead), i18nc("@action:button", "Read")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsUnRead), i18nc("@action:button", "Unread")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsImportant), i18nc("@action:button", "Important")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsIgnored), i18nc("@action:button", "Ignored")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsHam), i18nc("@action:button", "Ham")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsSpam), i18nc("@action:button", "Spam")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsWatched), i18nc("@action:button", "Watched")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsReplied), i18nc("@action:button", "Replied")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsForwarded), i18nc("@action:button", "Forwarded")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsEncrypted), i18nc("@action:button", "Encrypted")},
        {true, SearchLineCommand::searchLineTypeToString(SearchLineCommand::IsQueued), i18nc("@action:button", "Queued")},
#if 0 // Reactivate when we implemented it.
        {false, QStringLiteral("smaller:"), i18nc("@action:button", "Smaller")},
        {false, QStringLiteral("larger:"), i18nc("@action:button", "Larger")},
        {false, QStringLiteral("size:"), i18nc("@action:button", "Size")},
        {false, QStringLiteral("older_than:"), i18nc("@action:button", "Older")},
        {false, QStringLiteral("newer_than:"), i18nc("@action:button", "Newer")},
        {false, QStringLiteral("category:"), i18nc("@action:button", "Category")},
#endif
    };
}

#include "moc_searchlinecommandbuttonswidget.cpp"
