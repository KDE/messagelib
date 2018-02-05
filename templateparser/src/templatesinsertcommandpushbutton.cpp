/*
 * Copyright (C) 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "templatescommandmenu.h"
#include "templatesinsertcommandpushbutton.h"

#include <QAction>
#include <KActionMenu>
#include "templateparser_debug.h"
#include <KLocalizedString>


using namespace TemplateParser;


TemplatesInsertCommandPushButton::TemplatesInsertCommandPushButton(QWidget *parent, const QString &name)
    : QPushButton(parent)
{
    setObjectName(name);
    setText(i18n("&Insert Command"));

    mMenuCommand = new TemplatesCommandMenu(this);
    mMenuCommand->setObjectName(QStringLiteral("templatescommandmenu"));
    connect(mMenuCommand, QOverload<const QString &, int>::of(&TemplatesCommandMenu::insertCommand), this, &TemplatesInsertCommandPushButton::insertCommand);

    setMenu(mMenuCommand->menu());
    setToolTip(
        i18nc("@info:tooltip",
              "Select a command to insert into the template"));
    setWhatsThis(
        i18nc("@info:whatsthis",
              "Traverse this menu to find a command to insert into the current template "
              "being edited.  The command will be inserted at the cursor location, "
              "so you want to move your cursor to the desired insertion point first."));

}

TemplatesInsertCommandPushButton::~TemplatesInsertCommandPushButton()
{
}

