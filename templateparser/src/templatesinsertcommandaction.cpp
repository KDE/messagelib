/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandaction.h"

#include <QMenu>

using namespace TemplateParser;

TemplatesInsertCommandAction::TemplatesInsertCommandAction(QObject *parent)
    : QWidgetAction(parent)
    , mMenuCommand(new TemplatesCommandMenu(this))
{
    mMenuCommand->fillMenu();
    mMenuCommand->fillSubMenus();
    mMenuCommand->setObjectName(QLatin1StringView("templatescommandmenu"));
    connect(mMenuCommand, qOverload<const QString &, int>(&TemplatesCommandMenu::insertCommand), this, &TemplatesInsertCommandAction::insertCommand);
    setMenu(mMenuCommand->menu());
}

TemplatesInsertCommandAction::~TemplatesInsertCommandAction() = default;

TemplatesCommandMenu::MenuTypes TemplatesInsertCommandAction::type() const
{
    return mMenuCommand->type();
}

void TemplatesInsertCommandAction::setType(TemplatesCommandMenu::MenuTypes type)
{
    mMenuCommand->setType(type);
    setMenu(mMenuCommand->menu());
}

#include "moc_templatesinsertcommandaction.cpp"
