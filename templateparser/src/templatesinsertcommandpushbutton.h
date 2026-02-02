/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"
#include <QPushButton>
#include <TemplateParser/TemplatesCommandMenu>

namespace TemplateParser
{
/*!
 * \class TemplateParser::TemplatesInsertCommandPushButton
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/TemplatesInsertCommandPushButton
 * \brief The TemplatesInsertCommandPushButton class
 * \author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandPushButton : public QPushButton
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent widget
     * \param name button name
     */
    explicit TemplatesInsertCommandPushButton(QWidget *parent, const QString &name = QString());
    /*!
     * \brief Destructor
     */
    ~TemplatesInsertCommandPushButton() override;

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
    void insertCommand(const QString &cmd, int adjustCursor = 0);

private:
    TemplatesCommandMenu *const mMenuCommand;
};
}
