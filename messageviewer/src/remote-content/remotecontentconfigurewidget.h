/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QWidget>

#include "messageviewer_private_export.h"
#include "remotecontentinfo.h"
#include <QTreeWidgetItem>
class QTreeWidget;
namespace MessageViewer
{
class RemoteContentStatusTypeComboBox;
class RemoteContentInfo;
/**
 * @brief The RemoteContentWidgetItem class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentWidgetItem : public QTreeWidgetItem
{
public:
    enum ColumnType {
        Domain = 0,
        RuleType = 1,
    };

    explicit RemoteContentWidgetItem(QTreeWidget *parent = nullptr);
    ~RemoteContentWidgetItem() override;

    [[nodiscard]] MessageViewer::RemoteContentInfo::RemoteContentInfoStatus status() const;
    void setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus type);

private:
    MESSAGEVIEWER_NO_EXPORT void updateInfo();
    RemoteContentStatusTypeComboBox *const mStatusTypeCombobox;
};

class MESSAGEVIEWER_TESTS_EXPORT RemoteContentConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureWidget(QWidget *parent = nullptr);
    ~RemoteContentConfigureWidget() override;
    void saveSettings();

private:
    MESSAGEVIEWER_NO_EXPORT void insertRemoteContentInfo(const RemoteContentInfo &info);
    MESSAGEVIEWER_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &);
    MESSAGEVIEWER_NO_EXPORT void modifyRemoteContent(RemoteContentWidgetItem *rulesItem);
    MESSAGEVIEWER_NO_EXPORT void slotAdd();
    MESSAGEVIEWER_NO_EXPORT void fillContentInfo(const RemoteContentInfo &info, RemoteContentWidgetItem *item);
    MESSAGEVIEWER_NO_EXPORT void readSettings();
    QTreeWidget *const mTreeWidget;
};
}
