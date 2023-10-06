/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "utils/configurethemesdialog.h"
class QListWidgetItem;
namespace MessageList
{
namespace Core
{
class Theme;
}
namespace Utils
{
class ThemeEditor;
class ThemeListWidget;
class ThemeListWidgetItem;

class Q_DECL_HIDDEN ConfigureThemesDialog::ConfigureThemesDialogPrivate
{
public:
    ConfigureThemesDialogPrivate(ConfigureThemesDialog *owner)
        : q(owner)
    {
    }

    void fillThemeList();
    [[nodiscard]] QString uniqueNameForTheme(const QString &baseName, Core::Theme *skipTheme = nullptr);
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
