/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidgetAction>
#include <TemplateParser/TemplatesCommandMenu>

#include "templateparser_export.h"
namespace TemplateParser
{
/*!
 * \class TemplateParser::TemplatesInsertCommandAction
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplatesInsertCommandAction
 * \brief The TemplatesInsertCommandAction class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandAction : public QWidgetAction
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit TemplatesInsertCommandAction(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~TemplatesInsertCommandAction() override;

    /*!
     */
    /*!
     * \brief Returns the menu type
     * \return menu type
     */
    [[nodiscard]] TemplatesCommandMenu::MenuTypes type() const;
    /*!
     * \brief Sets the menu type
     * \param type the menu type to set
     */
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    /*!
     * \brief Emitted when a command is inserted
     * \param cmd the command string
     * \param adjustCursor cursor adjustment value
     */
    void insertCommand(const QString &cmd, int adjustCursor);

private:
    TemplatesCommandMenu *const mMenuCommand;
};
}
