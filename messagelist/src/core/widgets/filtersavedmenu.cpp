/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmenu.h"
using namespace MessageList::Core;

FilterSavedMenu::FilterSavedMenu(QWidget *parent)
    : QMenu(parent)
{
    connect(this, &FilterSavedMenu::aboutToShow, this, &FilterSavedMenu::slotShowMenu);
}

FilterSavedMenu::~FilterSavedMenu()
{
}

void FilterSavedMenu::slotShowMenu()
{
    clear();

    // TODO load filter
}
