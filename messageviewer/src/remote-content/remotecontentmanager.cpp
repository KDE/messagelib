/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmanager.h"
#include "remotecontentinfo.h"
using namespace MessageViewer;
RemoteContentManager::RemoteContentManager(QObject *parent)
    : QObject(parent)
{

}

RemoteContentManager::~RemoteContentManager()
{

}

RemoteContentManager *RemoteContentManager::self()
{
    static RemoteContentManager s_self;
    return &s_self;
}

void RemoteContentManager::loadSettings()
{
    //TODO
}

void RemoteContentManager::writeSettings()
{
    //TODO
}
