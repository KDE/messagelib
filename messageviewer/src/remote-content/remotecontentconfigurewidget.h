/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREWIDGET_H
#define REMOTECONTENTCONFIGUREWIDGET_H

#include <QWidget>

#include "messageviewer_private_export.h"
#include "remotecontentinfo.h"
#include <QTreeWidgetItem>
class QTreeWidget;
class QComboBox;
namespace MessageViewer
{
class RemoteContentInfo;
class MESSAGEVIEWER_EXPORT RemoteContentWidgetItem : public QTreeWidgetItem
{
public:
    enum ColumnType {
        Domain = 0,
        RuleType = 1,
    };

    explicit RemoteContentWidgetItem(QTreeWidget *parent = nullptr);
    ~RemoteContentWidgetItem() override;

    Q_REQUIRED_RESULT MessageViewer::RemoteContentInfo::RemoteContentInfoStatus status() const;
    void setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus type);

private:
    void updateInfo();
    QComboBox *const mStatusTypeCombobox;
};

class MESSAGEVIEWER_TESTS_EXPORT RemoteContentConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureWidget(QWidget *parent = nullptr);
    ~RemoteContentConfigureWidget() override;
    void saveSettings();

private:
    void insertRemoteContentInfo(const RemoteContentInfo &info);
    void slotCustomContextMenuRequested(const QPoint &);
    void modifyRemoteContent(RemoteContentWidgetItem *rulesItem);
    void slotAdd();
    void readSettings();
    QTreeWidget *const mTreeWidget;
};
}

#endif // REMOTECONTENTCONFIGUREWIDGET_H
