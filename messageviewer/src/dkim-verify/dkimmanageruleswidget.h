/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <MessageViewer/DKIMRule>
#include <QTreeWidgetItem>
#include <QWidget>
class QTreeWidget;
namespace MessageViewer
{
class DKIMManageRulesComboBox;
/**
 * @brief The DKIMManageRulesWidgetItem class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManageRulesWidgetItem : public QTreeWidgetItem
{
public:
    enum ColumnType {
        Enabled = 0,
        Domain = 1,
        ListId = 2,
        From = 3,
        SDid = 4,
        RuleType = 5,
        Priority = 6,
    };

    explicit DKIMManageRulesWidgetItem(QTreeWidget *parent = nullptr);
    ~DKIMManageRulesWidgetItem() override;

    Q_REQUIRED_RESULT MessageViewer::DKIMRule rule() const;
    void setRule(const MessageViewer::DKIMRule &rule);

private:
    void updateInfo();
    MessageViewer::DKIMRule mRule;
    DKIMManageRulesComboBox *const mRuleTypeCombobox;
};

/**
 * @brief The DKIMManageRulesWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManageRulesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DKIMManageRulesWidget(QWidget *parent = nullptr);
    ~DKIMManageRulesWidget() override;
    void loadSettings();
    void saveSettings();
    Q_REQUIRED_RESULT QByteArray saveHeaders() const;
    void restoreHeaders(const QByteArray &header);

    void addRule();
    Q_REQUIRED_RESULT QVector<MessageViewer::DKIMRule> rules() const;

private:
    void modifyRule(DKIMManageRulesWidgetItem *rulesItem);
    void slotCustomContextMenuRequested(const QPoint &);
    void duplicateRule(DKIMManageRulesWidgetItem *rulesItem);
    QTreeWidget *const mTreeWidget;
};
}
