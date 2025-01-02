/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

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
    , mSaveAction(new QAction(QIcon::fromTheme(QStringLiteral("document-save-as")), i18nc("@action", "Save Filter..."), this))
    , mConfigureAction(new QAction(QIcon::fromTheme(QStringLiteral("settings-configure")), i18nc("@action", "Configure..."), this))
{
    connect(this, &FilterSavedMenu::aboutToShow, this, &FilterSavedMenu::slotShowMenu);
}

FilterSavedMenu::~FilterSavedMenu() = default;

void FilterSavedMenu::slotShowMenu()
{
    for (auto act : std::as_const(mListAction)) {
        removeAction(act);
    }
    qDeleteAll(mListAction);
    mListAction.clear();
    removeAction(mSaveAction);
    removeAction(mConfigureAction);
    clear();
    FilterSavedManager::self()->loadMenu(this);
    if (!isEmpty()) {
        addSeparator();
    }
    addAction(mSaveAction);
    addSeparator();
    addAction(mConfigureAction);
    connect(mSaveAction, &QAction::triggered, this, &FilterSavedMenu::saveFilter);
    connect(mConfigureAction, &QAction::triggered, this, &FilterSavedMenu::configureFilters);
}

#include "moc_filtersavedmenu.cpp"
