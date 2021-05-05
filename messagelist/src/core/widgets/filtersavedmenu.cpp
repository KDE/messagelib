/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmenu.h"
#include <KLocalizedString>
#include <QAction>
using namespace MessageList::Core;

FilterSavedMenu::FilterSavedMenu(QWidget *parent)
    : QMenu(parent)
    , mSaveAction(new QAction(i18n("Save Filter..."), this))
    , mConfigureAction(new QAction(i18n("Configure..."), this))
{
    connect(this, &FilterSavedMenu::aboutToShow, this, &FilterSavedMenu::slotShowMenu);
    connect(mSaveAction, &QAction::triggered, this, &FilterSavedMenu::saveFilter);
    connect(mConfigureAction, &QAction::triggered, this, &FilterSavedMenu::configureFilters);
}

FilterSavedMenu::~FilterSavedMenu()
{
}

void FilterSavedMenu::slotShowMenu()
{
    // TODO
    mListAction.clear();
    clear();
}
