/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_private_export.h"
#include <MessageCore/DKIMRule>
#include <QComboBox>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMManageRulesComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DKIMManageRulesComboBox(QWidget *parent = nullptr);
    ~DKIMManageRulesComboBox() override;
    [[nodiscard]] MessageCore::DKIMRule::RuleType ruleType() const;
    void setRuleType(MessageCore::DKIMRule::RuleType type);

private:
    MESSAGEVIEWER_NO_EXPORT void init();
};
}
