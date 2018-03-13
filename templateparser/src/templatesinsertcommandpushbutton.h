/*
 * Copyright (C) 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * Copyright (C) 2018 Laurent Montel <montel@kde.org>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef TEMPLATEPARSER_TEMPLATESINSERTCOMMAND_H
#define TEMPLATEPARSER_TEMPLATESINSERTCOMMAND_H

#include <QPushButton>
#include "templateparser_export.h"
#include <TemplateParser/TemplatesCommandMenu>

class KActionMenu;

namespace TemplateParser {
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandPushButton(QWidget *parent, const QString &name = QString());
    ~TemplatesInsertCommandPushButton();

    TemplatesCommandMenu::MenuTypes type() const;
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    void insertCommand(const QString &cmd, int adjustCursor = 0);

private:
    TemplatesCommandMenu *mMenuCommand = nullptr;
};
}

#endif
