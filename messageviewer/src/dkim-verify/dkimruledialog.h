/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMRule>
#include <QDialog>
class QPushButton;
namespace MessageViewer
{
class DKIMRuleWidget;
class MESSAGEVIEWER_TESTS_EXPORT DKIMRuleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DKIMRuleDialog(QWidget *parent = nullptr);
    ~DKIMRuleDialog() override;

    [[nodiscard]] MessageViewer::DKIMRule rule() const;
    void loadRule(const MessageViewer::DKIMRule &rule);

private:
    MESSAGEVIEWER_NO_EXPORT void writeConfig();
    MESSAGEVIEWER_NO_EXPORT void readConfig();
    DKIMRuleWidget *const mRuleWidget;
    QPushButton *mOkButton = nullptr;
};
}
