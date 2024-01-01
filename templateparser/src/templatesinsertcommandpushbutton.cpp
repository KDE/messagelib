/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "templatesinsertcommandpushbutton.h"

#include "templateparser_debug.h"
#include <KLocalizedString>

using namespace TemplateParser;

TemplatesInsertCommandPushButton::TemplatesInsertCommandPushButton(QWidget *parent, const QString &name)
    : QPushButton(parent)
    , mMenuCommand(new TemplatesCommandMenu(this))
{
    setObjectName(name);
    setText(i18n("&Insert Command"));

    mMenuCommand->setObjectName(QLatin1StringView("templatescommandmenu"));
    mMenuCommand->fillMenu();
    mMenuCommand->fillSubMenus();
    setMenu(mMenuCommand->menu());
    connect(mMenuCommand, qOverload<const QString &, int>(&TemplatesCommandMenu::insertCommand), this, &TemplatesInsertCommandPushButton::insertCommand);

    setToolTip(i18nc("@info:tooltip", "Select a command to insert into the template"));
    setWhatsThis(i18nc("@info:whatsthis",
                       "Traverse this menu to find a command to insert into the current template "
                       "being edited.  The command will be inserted at the cursor location, "
                       "so you want to move your cursor to the desired insertion point first."));
}

TemplatesInsertCommandPushButton::~TemplatesInsertCommandPushButton() = default;

TemplatesCommandMenu::MenuTypes TemplatesInsertCommandPushButton::type() const
{
    return mMenuCommand->type();
}

void TemplatesInsertCommandPushButton::setType(TemplatesCommandMenu::MenuTypes type)
{
    mMenuCommand->setType(type);
    setMenu(mMenuCommand->menu());
}

#include "moc_templatesinsertcommandpushbutton.cpp"
