/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmenu.h"
#include <KLocalizedString>

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

void RemoteContentMenu::slotConfigure()
{
    //TODO
}
