/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimmanagerauthenticationserver.h"
#include "dkimutil.h"
#include <KConfigGroup>
#include <KSharedConfig>

using namespace MessageViewer;

DKIMManagerAuthenticationServer::DKIMManagerAuthenticationServer(QObject *parent)
    : QObject(parent)
{

}

DKIMManagerAuthenticationServer::~DKIMManagerAuthenticationServer()
{
    save();
}


DKIMManagerAuthenticationServer *DKIMManagerAuthenticationServer::self()
{
    static DKIMManagerAuthenticationServer s_self;
    return &s_self;
}

QStringList DKIMManagerAuthenticationServer::serverList() const
{
    return mServerList;
}

void DKIMManagerAuthenticationServer::setServerList(const QStringList &serverList)
{
    mServerList = serverList;
}

void DKIMManagerAuthenticationServer::load()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    KConfigGroup grp(config, "AuthenticationServer");
    mServerList = grp.readEntry("ServerList", QStringList());
}

void DKIMManagerAuthenticationServer::save()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    KConfigGroup grp(config, "AuthenticationServer");
    grp.writeEntry("ServerList", mServerList);
}
