/* SPDX-FileCopyrightText: 2009 James Bendig <james@imptalk.com>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

namespace MessageList
{
namespace Core
{
class Theme;
} // namespace Core

namespace Utils
{
class ThemeComboBox;

class ThemeComboBoxPrivate
{
public:
    explicit ThemeComboBoxPrivate(ThemeComboBox *owner)
        : q(owner)
    {
    }

    ThemeComboBox *const q;

    /**
     * Refresh list of themes in the combobox.
     */
    void slotLoadThemes();

    void setCurrentTheme(const Core::Theme *theme);
};
} // namespace Utils
} // namespace MessageList
