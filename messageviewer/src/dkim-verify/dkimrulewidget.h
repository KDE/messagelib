/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRULEWIDGET_H
#define DKIMRULEWIDGET_H
#include <QWidget>
#include "messageviewer_private_export.h"
#include <MessageViewer/DKIMRule>
class QCheckBox;
class QLineEdit;
class QSpinBox;
namespace MessageViewer {
class DKIMManageRulesComboBox;
class MESSAGEVIEWER_TESTS_EXPORT DKIMRuleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMRuleWidget(QWidget *parent = nullptr);
    ~DKIMRuleWidget() override;
    void loadRule(const MessageViewer::DKIMRule &rule);
    Q_REQUIRED_RESULT MessageViewer::DKIMRule rule() const;

Q_SIGNALS:
    void updateOkButtonRequested(bool enabled);

private:
    void updateOkButton();
    QCheckBox *mEnabled = nullptr;
    QLineEdit *mDomain = nullptr;
    QLineEdit *mSignatureDomainIdentifier = nullptr;
    QLineEdit *mFrom = nullptr;
    QLineEdit *mListId = nullptr;
    QSpinBox *mPriority = nullptr;
    DKIMManageRulesComboBox *mRuleType = nullptr;
};
}
#endif // DKIMRULEWIDGET_H
