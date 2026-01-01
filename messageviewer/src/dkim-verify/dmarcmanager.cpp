/*
   SPDX-FileCopyrightText: 2020-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcmanager.h"
using namespace Qt::Literals::StringLiterals;

#include "dkimutil.h"
#include <KConfigGroup>
#include <KSharedConfig>
using namespace MessageViewer;
DMARCManager::DMARCManager()
{
    loadNoServerKeys();
}

DMARCManager::~DMARCManager()
{
    saveNoServerKeys();
}

DMARCManager *DMARCManager::self()
{
    static DMARCManager s_self;
    return &s_self;
}

bool DMARCManager::isNoDMarcServerAddress(const QString &address) const
{
    return mNoDMarcServer.contains(address);
}

void DMARCManager::addNoDMarcServerAddress(const QString &address)
{
    if (!mNoDMarcServer.contains(address)) {
        mNoDMarcServer.append(address);
    }
}

void DMARCManager::saveNoServerKeys()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    KConfigGroup grp(config, u"NoExistingDmarcServer"_s);
    grp.writeEntry("AddressList", mNoDMarcServer);
}

void DMARCManager::loadNoServerKeys()
{
    const KSharedConfig::Ptr &config = KSharedConfig::openConfig(MessageViewer::DKIMUtil::defaultConfigFileName(), KConfig::NoGlobals);
    KConfigGroup grp(config, u"NoExistingDmarcServer"_s);
    mNoDMarcServer = grp.readEntry("AddressList", QStringList());
}
