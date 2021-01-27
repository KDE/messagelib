/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTMENU_H
#define REMOTECONTENTMENU_H

#include "messageviewer_private_export.h"
#include <QMenu>
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT RemoteContentMenu : public QMenu
{
public:
    explicit RemoteContentMenu(QWidget *parent = nullptr);
    ~RemoteContentMenu() override;
    void fillMenu();

private:
    void initialize();
    void slotConfigure();
    QAction *mConfigureRemoteContentAction = nullptr;
};
}
#endif // REMOTECONTENTMENU_H
