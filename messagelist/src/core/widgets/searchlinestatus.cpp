/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "searchlinestatus.h"
#include <KLocalizedString>
#include <QAction>

using namespace MessageList::Core;
SearchLineStatus::SearchLineStatus(QWidget *parent)
    : PimCommon::LineEditWithCompleter(parent),
      mLocked(false)
{
    initializeActions();
}

SearchLineStatus::~SearchLineStatus()
{

}

void SearchLineStatus::updateLockAction()
{
    if (mLocked) {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-locked")));
        mLockAction->setToolTip(i18nc("@info:tooltip",
                                      "Prevent the quick search field from being cleared when changing folders"));
    } else {
        mLockAction->setIcon(QIcon::fromTheme(QStringLiteral("object-unlocked")));
        mLockAction->setToolTip(i18nc("@info:tooltip", "Clear the quick search field when changing folders"));
    }
}

void SearchLineStatus::setLocked(bool b)
{
    if (mLocked != b) {
        slotToggledLockAction();
    }
}

void SearchLineStatus::initializeActions()
{
    mLockAction = addAction(QIcon::fromTheme(QStringLiteral("object-locked")), QLineEdit::TrailingPosition);
    connect(mLockAction, &QAction::triggered, this, &SearchLineStatus::slotToggledLockAction);
    updateLockAction();
}

void SearchLineStatus::slotToggledLockAction()
{
    mLocked = !mLocked;
    updateLockAction();
}
