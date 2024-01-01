/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidgetAction>
#include <TemplateParser/TemplatesCommandMenu>

#include "templateparser_export.h"
namespace TemplateParser
{
/**
 * @brief The TemplatesInsertCommandAction class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandAction(QObject *parent = nullptr);
    ~TemplatesInsertCommandAction() override;

    [[nodiscard]] TemplatesCommandMenu::MenuTypes type() const;
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    void insertCommand(const QString &cmd, int adjustCursor);

private:
    TemplatesCommandMenu *const mMenuCommand;
};
}
