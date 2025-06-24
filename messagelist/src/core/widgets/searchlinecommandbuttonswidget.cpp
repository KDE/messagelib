/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchlinecommandbuttonswidget.h"
using namespace Qt::Literals::StringLiterals;

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
    flowLayout->setObjectName(u"flowLayout"_s);
    flowLayout->setContentsMargins({});
    flowLayout->setSpacing(0);
    flowLayout->setHorizontalSpacing(0);
    flowLayout->setVerticalSpacing(0);
    const QList<SearchLineCommandButtonsWidget::ButtonInfo> buttonsList = fillCommandLineText();
    Q_ASSERT(!buttonsList.isEmpty());
    for (const auto &info : std::as_const(buttonsList)) {
        flowLayout->addWidget(createPushButton(info.i18n, info.identifier));
    }
}

SearchLineCommandButtonsWidget::~SearchLineCommandButtonsWidget() = default;

QPushButton *SearchLineCommandButtonsWidget::createPushButton(const QString &i18nStr, const QString &commandStr)
{
    auto pushButton = new QPushButton(i18nStr, this);
    pushButton->setObjectName(commandStr);
    pushButton->setToolTip(commandStr);
    connect(pushButton, &QPushButton::clicked, this, [this, commandStr]() {
        Q_EMIT insertCommand(commandStr);
    });
    return pushButton;
}

QList<SearchLineCommandButtonsWidget::ButtonInfo> SearchLineCommandButtonsWidget::fillCommandLineText() const
{
    const QList<SearchLineCommandButtonsWidget::ButtonInfo> buttonInfo = {
        {SearchLineCommand::generateCommandText(SearchLineCommand::Subject), i18nc("@action:button", "Subject")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::Body), i18nc("@action:button", "Body")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::From), i18nc("@action:button", "From")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::To), i18nc("@action:button", "To")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::Cc), i18nc("@action:button", "Cc")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::Bcc), i18nc("@action:button", "Bcc")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::HasAttachment), i18nc("@action:button", "Has Attachment")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::HasInvitation), i18nc("@action:button", "Has Invitation")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsRead), i18nc("@action:button", "Read")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsUnRead), i18nc("@action:button", "Unread")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsImportant), i18nc("@action:button", "Important")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsIgnored), i18nc("@action:button", "Ignored")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsHam), i18nc("@action:button", "Ham")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsSpam), i18nc("@action:button", "Spam")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsWatched), i18nc("@action:button", "Watched")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsReplied), i18nc("@action:button", "Replied")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsForwarded), i18nc("@action:button", "Forwarded")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsEncrypted), i18nc("@action:button", "Encrypted")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsQueued), i18nc("@action:button", "Queued")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsSent), i18nc("@action:button", "Sent")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsDeleted), i18nc("@action:button", "Deleted")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::IsAction), i18nc("@action:button", "Action")},
        {SearchLineCommand::generateCommandText(SearchLineCommand::Category), i18nc("@action:button", "Category")},
#if 0 // Reactivate when we implemented it.
        {false, u"smaller:"_s, i18nc("@action:button", "Smaller")},
        {false, u"larger:"_s, i18nc("@action:button", "Larger")},
        {false, u"size:"_s, i18nc("@action:button", "Size")},
        {false, u"older_than:"_s, i18nc("@action:button", "Older")},
        {false, u"newer_than:"_s, i18nc("@action:button", "Newer")},
#endif
    };
    return buttonInfo;
}

#include "moc_searchlinecommandbuttonswidget.cpp"
