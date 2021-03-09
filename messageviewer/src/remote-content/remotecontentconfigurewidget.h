/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREWIDGET_H
#define REMOTECONTENTCONFIGUREWIDGET_H

#include <QWidget>

#include "messageviewer_private_export.h"
class QTreeWidget;
class QListWidgetItem;
namespace MessageViewer
{
class RemoteContentInfo;
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
    void modifyRemoteContent(QListWidgetItem *rulesItem);
    void slotAdd();
    void readSettings();
    QTreeWidget *const mTreeWidget;
};
}

#endif // REMOTECONTENTCONFIGUREWIDGET_H
