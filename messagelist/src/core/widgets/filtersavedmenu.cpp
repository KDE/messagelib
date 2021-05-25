/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmenu.h"
#include "core/filtersavedmanager.h"
#include <KLocalizedString>
#include <QAction>
#include <QIcon>
using namespace MessageList::Core;

FilterSavedMenu::FilterSavedMenu(QWidget *parent)
    : QMenu(parent)
    , mSaveAction(new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18n("Save Filter..."), this))
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
    // TODO load current Filter saved action
    mListAction.clear();
    FilterSavedManager::self()->loadMenu(this);
    if (isEmpty()) {
        addSeparator();
        addAction(mSaveAction);
        addSeparator();
        addAction(mConfigureAction);
    }
}
