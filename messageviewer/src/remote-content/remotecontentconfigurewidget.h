/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTCONFIGUREWIDGET_H
#define REMOTECONTENTCONFIGUREWIDGET_H

#include <QWidget>

#include "messageviewer_private_export.h"
class QListWidget;
class QListWidgetItem;
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RemoteContentConfigureWidget(QWidget *parent = nullptr);
    ~RemoteContentConfigureWidget() override;
    void saveSettings();
private:
    void slotCustomContextMenuRequested(const QPoint &);
    void modifyRule(QListWidgetItem *rulesItem);
    void slotAdd();
    void readSettings();
    QListWidget *mListWidget = nullptr;
};
}

#endif // REMOTECONTENTCONFIGUREWIDGET_H
