/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
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
