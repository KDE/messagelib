/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REMOTECONTENTMENU_H
#define REMOTECONTENTMENU_H

#include "messageviewer_export.h"
#include <QMenu>
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT RemoteContentMenu : public QMenu
{
public:
    explicit RemoteContentMenu(QWidget *parent = nullptr);
    ~RemoteContentMenu() override;

    void updateMenu();

    Q_REQUIRED_RESULT QStringList urls() const;
    void setUrls(const QStringList &urls);

    void clearUrls();
    void appendUrl(const QString &url);

private:
    void initialize();
    void authorize(const QString &url);
    void slotConfigure();
    QStringList mUrls;
    QAction *mConfigureRemoteContentAction = nullptr;
};
}
#endif // REMOTECONTENTMENU_H
