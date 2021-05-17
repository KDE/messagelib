/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QDialog>
namespace MessageList
{
namespace Core
{
class ConfigureFiltersWidget;
class MESSAGELIST_TESTS_EXPORT ConfigureFiltersDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigureFiltersDialog(QWidget *parent = nullptr);
    ~ConfigureFiltersDialog() override;

private:
    void readConfig();
    void writeConfig();
    ConfigureFiltersWidget *const mConfigureFiltersWidget;
};

}
}
