/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMRule>
#include <QComboBox>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMManageRulesComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit DKIMManageRulesComboBox(QWidget *parent = nullptr);
    ~DKIMManageRulesComboBox() override;
    Q_REQUIRED_RESULT MessageViewer::DKIMRule::RuleType ruleType() const;
    void setRuleType(MessageViewer::DKIMRule::RuleType type);

private:
    void init();
};
}

