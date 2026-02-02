/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "messageviewer_export.h"
#include <memory>
class KActionCollection;
class QActionGroup;
class KActionMenu;
namespace MessageViewer
{
class HeaderStyle;
class HeaderStrategy;
class HeaderStylePluginPrivate;
class HeaderStyleInterface;
/*!
 * \class MessageViewer::HeaderStylePlugin
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/HeaderStylePlugin
 * \brief The HeaderStylePlugin class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT HeaderStylePlugin : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     * \param parent parent object
     */
    explicit HeaderStylePlugin(QObject *parent = nullptr);
    /*!
     * \brief Destructor
     */
    ~HeaderStylePlugin() override;

    /*!
     * \brief Returns the header style
     * \return HeaderStyle instance
     */
    virtual HeaderStyle *headerStyle() const = 0;
    /*!
     * \brief Returns the header strategy
     * \return HeaderStrategy instance
     */
    virtual HeaderStrategy *headerStrategy() const = 0;
    /*!
     * \brief Creates a view for the header
     * \param menu action menu
     * \param actionGroup action group
     * \param ac action collection
     * \param parent parent object
     * \return HeaderStyleInterface instance
     */
    virtual HeaderStyleInterface *createView(KActionMenu *menu, QActionGroup *actionGroup, KActionCollection *ac, QObject *parent = nullptr) = 0;
    /*!
     * \brief Returns the name of the header style
     * \return plugin name
     */
    [[nodiscard]] virtual QString name() const = 0;
    /*!
     * \brief Checks if the header style has margins
     * \return true if has margin
     */
    [[nodiscard]] virtual bool hasMargin() const;
    /*!
     * \brief Returns the alignment setting
     * \return alignment string
     */
    [[nodiscard]] virtual QString alignment() const;
    /*!
     * \brief Returns the elided text size
     * \return text size in pixels
     */
    [[nodiscard]] virtual int elidedTextSize() const;

    /*!
     * \brief Sets whether the plugin is enabled
     * \param enabled enable state
     */
    void setIsEnabled(bool enabled);
    /*!
     * \brief Returns whether the plugin is enabled
     * \return true if enabled
     */
    [[nodiscard]] bool isEnabled() const;

    /*!
     * \brief Returns the attachment HTML
     * \return HTML string
     */
    [[nodiscard]] virtual QString attachmentHtml() const;

    /*!
     * \brief Checks if the plugin has a configure dialog
     * \return true if has dialog
     */
    [[nodiscard]] virtual bool hasConfigureDialog() const;

    /*!
     * \brief Shows the configure dialog
     * \param parent parent widget
     */
    virtual void showConfigureDialog(QWidget *parent);

    /*!
     * \brief Returns extra CSS for screen display
     * \param headerFont header font name
     * \return CSS string
     */
    [[nodiscard]] virtual QString extraScreenCss(const QString &headerFont) const;
    /*!
     * \brief Returns extra CSS for printing
     * \param headerFont header font name
     * \return CSS string
     */
    [[nodiscard]] virtual QString extraPrintCss(const QString &headerFont) const;
    /*!
     * \brief Returns common extra CSS
     * \param headerFont header font name
     * \return CSS string
     */
    [[nodiscard]] virtual QString extraCommonCss(const QString &headerFont) const;

private:
    std::unique_ptr<HeaderStylePluginPrivate> const d;
};
}
