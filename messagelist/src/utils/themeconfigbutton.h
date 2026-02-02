/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include "messagelist_export.h"
#include <QPushButton>

namespace MessageList
{
namespace Utils
{
class ThemeComboBox;
class ThemeConfigButtonPrivate;

/*!
 * \class MessageList::Utils::ThemeConfigButton
 * \inmodule MessageList
 * \inheaderfile MessageList/ThemeConfigButton
 *
 * A specialized QPushButton that displays the theme
 * configure dialog when pressed.
 */
class MESSAGELIST_EXPORT ThemeConfigButton : public QPushButton
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor.
     * \param parent The parent widget for the button.
     * \param themeComboBox Optional ThemeComboBox to be kept in sync
     * with changes made by the configure dialog.
     */
    explicit ThemeConfigButton(QWidget *parent, const ThemeComboBox *themeComboBox = nullptr);
    /*!
     * \brief Destructor.
     */
    ~ThemeConfigButton() override;

Q_SIGNALS:
    /*!
     * \brief A signal emitted when configure dialog has been successfully completed.
     */
    void configureDialogCompleted();

private:
    std::unique_ptr<ThemeConfigButtonPrivate> const d;
};
} // namespace Utils
} // namespace MessageList
