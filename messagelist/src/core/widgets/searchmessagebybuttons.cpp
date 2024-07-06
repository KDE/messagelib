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
    auto button = new QToolButton(this);
    button->setText(text);
    mButtonGroup->addButton(button, option);
}

void SearchMessageByButtons::changeSearchAgainstFromOrToText()
{
#if 0
    if (mContainsOutboundMessages) {
        mSearchAgainstFromOrToAction->setText(i18n("To"));
    } else {
        mSearchAgainstFromOrToAction->setText(i18n("From"));
    }
#endif
}

#if 0

mSearchAgainstFromOrToAction = new QAction(mFilterMenu);
changeSearchAgainstFromOrToText();
grp->addAction(mSearchAgainstFromOrToAction);
mFilterMenu->addAction(mSearchAgainstFromOrToAction);
mSearchAgainstFromOrToAction->setCheckable(true);

connect(grp, &QActionGroup::triggered, this, &SearchLineStatus::slotFilterActionClicked);
#endif

#include "moc_searchmessagebybuttons.cpp"
