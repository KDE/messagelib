/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMRULEDIALOG_H
#define DKIMRULEDIALOG_H

#include "messageviewer_private_export.h"
#include <QDialog>
#include <MessageViewer/DKIMRule>
class QPushButton;
namespace MessageViewer {
class DKIMRuleWidget;
class MESSAGEVIEWER_TESTS_EXPORT DKIMRuleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DKIMRuleDialog(QWidget *parent = nullptr);
    ~DKIMRuleDialog() override;

    Q_REQUIRED_RESULT MessageViewer::DKIMRule rule() const;
    void loadRule(const MessageViewer::DKIMRule &rule);
private:
    void slotAccepted();
    void writeConfig();
    void readConfig();
    DKIMRuleWidget *mRuleWidget = nullptr;
    QPushButton *mOkButton = nullptr;
};
}

#endif // DKIMRULEDIALOG_H
