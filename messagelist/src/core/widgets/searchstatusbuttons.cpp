/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchstatusbuttons.h"

#include "messagelist_debug.h"
#include <KLocalizedString>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QToolButton>
using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
SearchStatusButtons::SearchStatusButtons(QWidget *parent)
    : QWidget{parent}
    , mButtonGroup(new QButtonGroup(this))
    , mMainLayout(new QHBoxLayout(this))
{
    mMainLayout->setObjectName(u"mainLayout"_s);
    mMainLayout->setContentsMargins({});
    mMainLayout->setSpacing(0);

    mButtonGroup->setObjectName(u"mButtonGroup"_s);
    mButtonGroup->setExclusive(false);

    connect(mButtonGroup, &QButtonGroup::idClicked, this, &SearchStatusButtons::updateFilters);
    createAction();
}

SearchStatusButtons::~SearchStatusButtons() = default;

void SearchStatusButtons::createAction()
{
    createFilterAction(QIcon::fromTheme(u"mail-unread"_s),
                       i18nc("@action:inmenu Status of a message", "Unread"),
                       Akonadi::MessageStatus::statusUnread().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-replied"_s),
                       i18nc("@action:inmenu Status of a message", "Replied"),
                       Akonadi::MessageStatus::statusReplied().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-forwarded"_s),
                       i18nc("@action:inmenu Status of a message", "Forwarded"),
                       Akonadi::MessageStatus::statusForwarded().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-mark-important"_s),
                       i18nc("@action:inmenu Status of a message", "Important"),
                       Akonadi::MessageStatus::statusImportant().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-task"_s),
                       i18nc("@action:inmenu Status of a message", "Action Item"),
                       Akonadi::MessageStatus::statusToAct().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-thread-watch"_s),
                       i18nc("@action:inmenu Status of a message", "Watched"),
                       Akonadi::MessageStatus::statusWatched().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-thread-ignored"_s),
                       i18nc("@action:inmenu Status of a message", "Ignored"),
                       Akonadi::MessageStatus::statusIgnored().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-attachment"_s),
                       i18nc("@action:inmenu Status of a message", "Has Attachment"),
                       Akonadi::MessageStatus::statusHasAttachment().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-invitation"_s),
                       i18nc("@action:inmenu Status of a message", "Has Invitation"),
                       Akonadi::MessageStatus::statusHasInvitation().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-mark-junk"_s),
                       i18nc("@action:inmenu Status of a message", "Spam"),
                       Akonadi::MessageStatus::statusSpam().toQInt32());

    createFilterAction(QIcon::fromTheme(u"mail-mark-notjunk"_s),
                       i18nc("@action:inmenu Status of a message", "Ham"),
                       Akonadi::MessageStatus::statusHam().toQInt32());
}

void SearchStatusButtons::createFilterAction(const QIcon &icon, const QString &text, int value)
{
    auto toolButton = new QToolButton(this);
    toolButton->setCheckable(true);
    toolButton->setAutoRaise(true);
    toolButton->setIcon(icon);
    toolButton->setToolTip(text);
    mButtonGroup->addButton(toolButton, value);
    mMainLayout->addWidget(toolButton, 0, Qt::AlignTop);
}

void SearchStatusButtons::clearFilter()
{
    for (auto button : mButtonGroup->buttons()) {
        button->setChecked(false);
    }
}

void SearchStatusButtons::updateFilters()
{
    QList<Akonadi::MessageStatus> lstStatus;

    for (auto button : mButtonGroup->buttons()) {
        if (button->isChecked()) {
            Akonadi::MessageStatus status;
            status.fromQInt32(static_cast<qint32>(mButtonGroup->id(button)));
            lstStatus.append(status);
        }
    }
    Q_EMIT filterStatusChanged(lstStatus);
}

void SearchStatusButtons::setFilterMessageStatus(const QList<Akonadi::MessageStatus> &newLstStatus)
{
    for (const Akonadi::MessageStatus &status : newLstStatus) {
        if (!mButtonGroup->button(status.toQInt32())) {
            qCWarning(MESSAGELIST_LOG) << " status not found " << status.toQInt32();
        } else {
            mButtonGroup->button(status.toQInt32())->setChecked(true);
        }
    }
}
#include "moc_searchstatusbuttons.cpp"
