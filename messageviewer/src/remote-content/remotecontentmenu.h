/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <QMenu>
namespace MessageViewer
{
/**
 * @brief The RemoteContentMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT RemoteContentMenu : public QMenu
{
    Q_OBJECT
public:
    explicit RemoteContentMenu(QWidget *parent = nullptr);
    ~RemoteContentMenu() override;

    void updateMenu();

    Q_REQUIRED_RESULT QStringList urls() const;
    void setUrls(const QStringList &urls);

    void clearUrls();
    void appendUrl(const QString &url);

Q_SIGNALS:
    void updateEmail();

private:
    void authorize(const QString &url);
    void slotConfigure();
    QStringList mUrls;
    QAction *const mConfigureRemoteContentAction;
    QList<QAction *> mListAction;
};
}
