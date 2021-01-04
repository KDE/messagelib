/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenu.h"
#include "remotecontentconfiguredialog.h"
#include <KLocalizedString>
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
    mConfigureRemoteContentAction = addAction(i18n("Configure"), this, &RemoteContentMenu::slotConfigure);
    //TODO
}

void RemoteContentMenu::fillMenu()
{
    //TODO
}

void RemoteContentMenu::slotConfigure()
{
    QPointer<MessageViewer::RemoteContentConfigureDialog> remoteContentDialog = new MessageViewer::RemoteContentConfigureDialog(this);
    if (remoteContentDialog->exec()) {
        //TODO ?
    }
    delete remoteContentDialog;
}
