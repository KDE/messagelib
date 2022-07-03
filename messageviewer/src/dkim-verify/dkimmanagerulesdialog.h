/*
   SPDX-FileCopyrightText: 2019-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_export.h"
#include <QDialog>

class QPushButton;
namespace MessageViewer
{
class DKIMManageRulesWidget;
/**
 * @brief The DKIMManageRulesDialog class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManageRulesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DKIMManageRulesDialog(QWidget *parent = nullptr);
    ~DKIMManageRulesDialog() override;

private:
    void slotAccepted();
    void readConfig();
    void writeConfig();
    void slotImport();
    void slotExport();
    DKIMManageRulesWidget *const mRulesWidget;
    QPushButton *const mExportButton;
};
}
