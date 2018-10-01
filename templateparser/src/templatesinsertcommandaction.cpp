/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "templatesinsertcommandaction.h"
using namespace TemplateParser;

TemplatesInsertCommandAction::TemplatesInsertCommandAction(QObject *parent)
    : QWidgetAction(parent)
{
    mMenuCommand = new TemplatesCommandMenu(this);
    mMenuCommand->fillMenu();
    mMenuCommand->fillSubMenus();
    mMenuCommand->setObjectName(QStringLiteral("templatescommandmenu"));
    connect(mMenuCommand, QOverload<const QString &, int>::of(&TemplatesCommandMenu::insertCommand), this, &TemplatesInsertCommandAction::insertCommand);
    setMenu(mMenuCommand->menu());
}

TemplatesInsertCommandAction::~TemplatesInsertCommandAction()
{
}

TemplatesCommandMenu::MenuTypes TemplatesInsertCommandAction::type() const
{
    return mMenuCommand->type();
}

void TemplatesInsertCommandAction::setType(TemplatesCommandMenu::MenuTypes type)
{
    mMenuCommand->setType(type);
    setMenu(mMenuCommand->menu());
}
