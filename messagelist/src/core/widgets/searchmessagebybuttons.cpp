/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "searchmessagebybuttons.h"
#include <KLocalizedString>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

using namespace MessageList::Core;
SearchMessageByButtons::SearchMessageByButtons(QWidget *parent)
    : QWidget{parent}
    , mMainLayout(new QHBoxLayout(this))
    , mButtonGroup(new QButtonGroup(this))
{
    mMainLayout->setObjectName(QStringLiteral("mainLayout"));
    mMainLayout->setContentsMargins({});
    mMainLayout->setSpacing(0);

    mButtonGroup->setObjectName(QStringLiteral("mButtonGroup"));
    connect(mButtonGroup, &QButtonGroup::idClicked, this, [this](int index) {
        // TODO
    });
    createButtons();
    mMainLayout->addStretch(0);
}

SearchMessageByButtons::~SearchMessageByButtons() = default;

void SearchMessageByButtons::createButtons()
{
    createFilterButton(i18nc("@action", "Full Message"), SearchEveryWhere);
    createFilterButton(i18nc("@action", "Body"), SearchAgainstBody);
    createFilterButton(i18nc("@action", "Subject"), SearchAgainstSubject);
    createFilterButton(QString(), SearchAgainstTo); // TODO update text
    createFilterButton(i18nc("@action", "BCC"), SearchAgainstBcc);
}

void SearchMessageByButtons::createFilterButton(const QString &text, SearchOption option)
{
    auto toolButton = new QToolButton(this);
    toolButton->setText(text);
    toolButton->setCheckable(true);
    toolButton->setAutoRaise(true);
    mButtonGroup->addButton(toolButton, option);
    mMainLayout->addWidget(toolButton, 0, Qt::AlignTop);
}

bool SearchMessageByButtons::containsOutboundMessages() const
{
    return mContainsOutboundMessages;
}

void SearchMessageByButtons::setContainsOutboundMessages(bool containsOutboundMessages)
{
    if (mContainsOutboundMessages != containsOutboundMessages) {
        mContainsOutboundMessages = containsOutboundMessages;
        changeSearchAgainstFromOrToText();
    }
}

void SearchMessageByButtons::changeSearchAgainstFromOrToText()
{
    // TODO
    if (mContainsOutboundMessages) {
        // mSearchAgainstFromOrToAction->setText(i18n("To"));
    } else {
        // mSearchAgainstFromOrToAction->setText(i18n("From"));
    }
}

#include "moc_searchmessagebybuttons.cpp"
