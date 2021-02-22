/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenu.h"
#include "remotecontentconfiguredialog.h"
#include <KLocalizedString>
#include <QAction>
#include <QPointer>

using namespace MessageViewer;
RemoteContentMenu::RemoteContentMenu(QWidget *parent)
    : QMenu(parent)
{
    initialize();
}

RemoteContentMenu::~RemoteContentMenu()
{
}

void RemoteContentMenu::initialize()
{
    mConfigureRemoteContentAction = new QAction(i18n("Configure"), this);
    connect(mConfigureRemoteContentAction, &QAction::triggered, this, &RemoteContentMenu::slotConfigure);
}

void RemoteContentMenu::slotConfigure()
{
    QPointer<MessageViewer::RemoteContentConfigureDialog> remoteContentDialog = new MessageViewer::RemoteContentConfigureDialog(this);
    remoteContentDialog->exec();
    delete remoteContentDialog;
}

void RemoteContentMenu::updateMenu()
{
    clear();
    addSeparator();
    addAction(mConfigureRemoteContentAction);
}
