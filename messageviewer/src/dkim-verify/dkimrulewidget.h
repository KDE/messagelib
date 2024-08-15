/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMRule>
#include <QWidget>
class QCheckBox;
class QLineEdit;
class QSpinBox;
namespace MessageViewer
{
class DKIMManageRulesComboBox;
class MESSAGEVIEWER_TESTS_EXPORT DKIMRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMRuleWidget(QWidget *parent = nullptr);
    ~DKIMRuleWidget() override;
    void loadRule(const MessageViewer::DKIMRule &rule);
    [[nodiscard]] MessageViewer::DKIMRule rule() const;

Q_SIGNALS:
    void updateOkButtonRequested(bool enabled);

private:
    MESSAGEVIEWER_NO_EXPORT void updateOkButton();
    QCheckBox *const mEnabled;
    QLineEdit *const mDomain;
    QLineEdit *const mSignatureDomainIdentifier;
    QLineEdit *const mFrom;
    QLineEdit *const mListId;
    QSpinBox *const mPriority;
    DKIMManageRulesComboBox *const mRuleType;
};
}
