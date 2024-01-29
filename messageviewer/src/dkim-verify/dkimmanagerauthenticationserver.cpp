/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerauthenticationserver.h"
#include "dkimutil.h"
#include <KConfigGroup>
#include <KSharedConfig>

using namespace MessageViewer;
namespace
{
static const char myDKIMManagerAuthenticationServerConfigGroupName[] = "AuthenticationServer";
}
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
    KConfigGroup grp(config, QLatin1StringView(myDKIMManagerAuthenticationServerConfigGroupName));
    mServerList = grp.readEntry("ServerList", QStringList());
}

void DKIMManagerAuthenticationServer::save()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    KConfigGroup grp(config, QLatin1StringView(myDKIMManagerAuthenticationServerConfigGroupName));
    grp.writeEntry("ServerList", mServerList);
}

#include "moc_dkimmanagerauthenticationserver.cpp"
