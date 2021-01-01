/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatesinsertcommandaction.h"
using namespace TemplateParser;

TemplatesInsertCommandAction::TemplatesInsertCommandAction(QObject *parent)
    : QWidgetAction(parent)
    , mMenuCommand(new TemplatesCommandMenu(this))
{
    mMenuCommand->fillMenu();
    mMenuCommand->fillSubMenus();
    mMenuCommand->setObjectName(QStringLiteral("templatescommandmenu"));
    connect(mMenuCommand, qOverload<const QString &, int>(&TemplatesCommandMenu::insertCommand), this, &TemplatesInsertCommandAction::insertCommand);
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
