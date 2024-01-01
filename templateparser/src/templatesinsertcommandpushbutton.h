/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"
#include <QPushButton>
#include <TemplateParser/TemplatesCommandMenu>

namespace TemplateParser
{
/**
 * @brief The TemplatesInsertCommandPushButton class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandPushButton(QWidget *parent, const QString &name = QString());
    ~TemplatesInsertCommandPushButton() override;

    [[nodiscard]] TemplatesCommandMenu::MenuTypes type() const;
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    void insertCommand(const QString &cmd, int adjustCursor = 0);

private:
    TemplatesCommandMenu *const mMenuCommand;
};
}
