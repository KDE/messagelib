/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef MESSAGELIST_UTILS_CONFIGURETHEMESDIALOG_P_H
#define MESSAGELIST_UTILS_CONFIGURETHEMESDIALOG_P_H

#include "utils/configurethemesdialog.h"
class QListWidgetItem;
namespace MessageList {
namespace Core {
class Theme;
}
namespace Utils {
class ThemeEditor;
class ThemeListWidget;
class ThemeListWidgetItem;

class Q_DECL_HIDDEN ConfigureThemesDialog::Private
{
public:
    Private(ConfigureThemesDialog *owner)
        : q(owner)
    {
    }

    void fillThemeList();
    QString uniqueNameForTheme(const QString &baseName, Core::Theme *skipTheme = nullptr);
    ThemeListWidgetItem *findThemeItemByName(const QString &name, Core::Theme *skipTheme = nullptr);
    ThemeListWidgetItem *findThemeItemByTheme(Core::Theme *set);
    ThemeListWidgetItem *findThemeItemById(const QString &themeId);
    void commitEditor();

    void themeListItemClicked(QListWidgetItem *);
    void newThemeButtonClicked();
    void cloneThemeButtonClicked();
    void deleteThemeButtonClicked();
    void editedThemeNameChanged();
    void okButtonClicked();
    void exportThemeButtonClicked();
    void importThemeButtonClicked();

    ConfigureThemesDialog *const q;

    ThemeListWidget *mThemeList = nullptr;
    ThemeEditor *mEditor = nullptr;
    QPushButton *mNewThemeButton = nullptr;
    QPushButton *mCloneThemeButton = nullptr;
    QPushButton *mDeleteThemeButton = nullptr;
    QPushButton *mExportThemeButton = nullptr;
    QPushButton *mImportThemeButton = nullptr;
};
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_CONFIGURESKINSDIALOG_P_H
