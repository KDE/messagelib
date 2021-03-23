/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QDialog>

#include <QListWidget>

namespace MessageList
{
namespace Utils
{
class ConfigureThemesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigureThemesDialog(QWidget *parent = nullptr);
    ~ConfigureThemesDialog() override;

    void selectTheme(const QString &themeId);

Q_SIGNALS:
    void okClicked();

private:
    class Private;
    Private *const d;
};
} // namespace Utils
} // namespace MessageList

