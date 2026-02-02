/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@ubiz.ru>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "templateparser_export.h"

#include <QObject>
#include <memory>

class KActionCollection;
class KActionMenu;

namespace TemplateParser
{
class CustomTemplatesMenuPrivate;
/*!
 * \class TemplateParser::CustomTemplatesMenu
 * \inmodule TemplateParser
 * \inheaderfile TemplateParser/CustomTemplatesMenu
 * \brief The CustomTemplatesMenu class
 */
class TEMPLATEPARSER_EXPORT CustomTemplatesMenu : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param parent parent widget
     * \param ac action collection
     */
    explicit CustomTemplatesMenu(QWidget *parent, KActionCollection *ac);
    /*!
     * \brief Destructor
     */
    ~CustomTemplatesMenu() override;

    /*!
     */
    /*!
     * \brief Returns the reply action menu
     * \return KActionMenu pointer
     */
    [[nodiscard]] KActionMenu *replyActionMenu() const;
    /*!
     */
    /*!
     * \brief Returns the reply-all action menu
     * \return KActionMenu pointer
     */
    [[nodiscard]] KActionMenu *replyAllActionMenu() const;
    /*!
     */
    /*!
     * \brief Returns the forward action menu
     * \return KActionMenu pointer
     */
    [[nodiscard]] KActionMenu *forwardActionMenu() const;

public Q_SLOTS:
    /*!
     * \brief Updates the menu
     */
    void update();

Q_SIGNALS:
    /*!
     * \brief Emitted when reply template is selected
     * \param tmpl template name
     */
    void replyTemplateSelected(const QString &tmpl);
    /*!
     * \brief Emitted when reply-all template is selected
     * \param tmpl template name
     */
    void replyAllTemplateSelected(const QString &tmpl);
    /*!
     * \brief Emitted when forward template is selected
     * \param tmpl template name
     */
    void forwardTemplateSelected(const QString &tmpl);

private:
    TEMPLATEPARSER_NO_EXPORT void slotReplySelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void slotReplyAllSelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void slotForwardSelected(int idx);
    TEMPLATEPARSER_NO_EXPORT void clear();
    std::unique_ptr<CustomTemplatesMenuPrivate> const d;
};
}
