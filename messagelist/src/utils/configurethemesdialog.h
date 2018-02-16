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

#ifndef MESSAGELIST_UTILS_CONFIGURETHEMESDIALOG_H
#define MESSAGELIST_UTILS_CONFIGURETHEMESDIALOG_H

#include <QDialog>

#include <QListWidget>

namespace MessageList {
namespace Utils {
class ConfigureThemesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureThemesDialog(QWidget *parent = nullptr);
    ~ConfigureThemesDialog();

    void selectTheme(const QString &themeId);

Q_SIGNALS:
    void okClicked();

private:
    class Private;
    Private *const d;
};
} // namespace Utils
} // namespace MessageList

#endif //!__MESSAGELIST_UTILS_CONFIGURESKINSDIALOG_H
