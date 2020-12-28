/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmanager.h"
#include "remotecontentinfo.h"
#include <KConfigGroup>
#include <KSharedConfig>

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

bool RemoteContentManager::isAutorized(const QString &url) const
{
    for (const RemoteContentInfo &info : qAsConst(mRemoveContentInfo)) {
        if (info.url() == url) {
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized;
        }
    }
    return false;
}

void RemoteContentManager::loadSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, "RemoteContent");
    const QStringList blockedUrl = group.readEntry("Blocked", QStringList());

    //TODO
}

void RemoteContentManager::writeSettings()
{
    //TODO
}
