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
    connect(mButtonGroup, &QButtonGroup::idClicked, this, &SearchMessageByButtons::searchOptionChanged);
    createButtons();
    mMainLayout->addStretch(0);
}

SearchMessageByButtons::~SearchMessageByButtons() = default;

void SearchMessageByButtons::createButtons()
{
    auto button = createFilterButton(i18nc("@action", "Full Message"), SearchEveryWhere);
    button->setChecked(true); // Enable by default
    createFilterButton(i18nc("@action", "Body"), SearchAgainstBody);
    createFilterButton(i18nc("@action", "Subject"), SearchAgainstSubject);
    mSearchAgainstFromOrToButton = createFilterButton(QString(), SearchAgainstTo);
    createFilterButton(i18nc("@action", "BCC"), SearchAgainstBcc);
    changeSearchAgainstFromOrToText();
}

QToolButton *SearchMessageByButtons::createFilterButton(const QString &text, SearchOption option)
{
    auto toolButton = new QToolButton(this);
    toolButton->setText(text);
    toolButton->setCheckable(true);
    toolButton->setAutoRaise(true);
    mButtonGroup->addButton(toolButton, option);
    mMainLayout->addWidget(toolButton, 0, Qt::AlignTop);
    return toolButton;
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
    if (mContainsOutboundMessages) {
        mSearchAgainstFromOrToButton->setText(i18n("To"));
    } else {
        mSearchAgainstFromOrToButton->setText(i18n("From"));
    }
}

SearchMessageByButtons::SearchOptions SearchMessageByButtons::searchOptions() const
{
    SearchMessageByButtons::SearchOptions searchOptions;
    const int checked = mButtonGroup->checkedId();
    if (checked == SearchAgainstTo) {
        if (mContainsOutboundMessages) {
            searchOptions |= SearchMessageByButtons::SearchAgainstTo;
        } else {
            searchOptions |= SearchMessageByButtons::SearchAgainstFrom;
        }
    } else {
        searchOptions |= static_cast<SearchMessageByButtons::SearchOption>(checked);
    }
    return searchOptions;
}

void SearchMessageByButtons::setSearchOptions(SearchMessageByButtons::SearchOptions opts)
{
    mButtonGroup->button(SearchMessageByButtons::SearchEveryWhere)->setChecked(opts & SearchMessageByButtons::SearchEveryWhere);
    mButtonGroup->button(SearchMessageByButtons::SearchAgainstBody)->setChecked(opts & SearchMessageByButtons::SearchAgainstBody);
    mButtonGroup->button(SearchMessageByButtons::SearchAgainstSubject)->setChecked(opts & SearchMessageByButtons::SearchAgainstSubject);
    mButtonGroup->button(SearchMessageByButtons::SearchAgainstBcc)->setChecked(opts & SearchMessageByButtons::SearchAgainstBcc);
    mButtonGroup->button(SearchMessageByButtons::SearchEveryWhere)->setChecked(opts & SearchMessageByButtons::SearchEveryWhere);

    if (mContainsOutboundMessages) {
        mButtonGroup->button(SearchMessageByButtons::SearchAgainstTo)->setChecked(opts & SearchMessageByButtons::SearchAgainstTo);
    } else {
        mButtonGroup->button(SearchMessageByButtons::SearchAgainstTo)->setChecked(opts & SearchMessageByButtons::SearchAgainstFrom);
    }
}

void SearchMessageByButtons::clearFilter()
{
    mButtonGroup->button(SearchMessageByButtons::SearchEveryWhere)->setChecked(true);
}

#include "moc_searchmessagebybuttons.cpp"
