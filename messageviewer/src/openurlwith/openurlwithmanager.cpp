/*
   SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "openurlwithmanager.h"

#include <KConfigGroup>
#include <KSharedConfig>
using namespace MessageViewer;
namespace
{
static const char myOpenUrlWithGroupName[] = "OpenUrlWith";
}
OpenUrlWithManager::OpenUrlWithManager(QObject *parent)
    : QObject{parent}
{
    loadSettings();
}

OpenUrlWithManager::~OpenUrlWithManager()
{
    writeSettings();
}

OpenUrlWithManager *OpenUrlWithManager::self()
{
    static OpenUrlWithManager s_self;
    return &s_self;
}

void OpenUrlWithManager::clear()
{
    mOpenWithUrlInfo.clear();
}

void OpenUrlWithManager::loadSettings()
{
#if 0
    mRemoveContentInfo.clear();
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, myRemoteContentGroupName);
    const QStringList blockedUrl = group.readEntry("Blocked", QStringList());
    const QStringList authorizedUrl = group.readEntry("Authorized", QStringList());
    for (const QString &url : blockedUrl) {
        RemoteContentInfo info;
        info.setUrl(url);
        info.setStatus(RemoteContentInfo::RemoteContentInfoStatus::Blocked);
        mRemoveContentInfo.append(info);
    }
    for (const QString &url : authorizedUrl) {
        RemoteContentInfo info;
        info.setUrl(url);
        info.setStatus(RemoteContentInfo::RemoteContentInfoStatus::Authorized);
        mRemoveContentInfo.append(info);
    }
#endif
}

void OpenUrlWithManager::writeSettings()
{
#if 0
    KConfigGroup group(config, myRemoteContentGroupName);
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    QStringList blockedUrl;
    QStringList authorizedUrl;
    for (const RemoteContentInfo &info : std::as_const(mRemoveContentInfo)) {
        switch (info.status()) {
        case RemoteContentInfo::RemoteContentInfoStatus::Unknown:
            break;
        case RemoteContentInfo::RemoteContentInfoStatus::Blocked:
            blockedUrl.append(info.url());
            break;
        case RemoteContentInfo::RemoteContentInfoStatus::Authorized:
            authorizedUrl.append(info.url());
            break;
        }
    }
    group.writeEntry("Blocked", blockedUrl);
    group.writeEntry("Authorized", authorizedUrl);
    group.sync();
#endif
}

const QVector<OpenWithUrlInfo> &OpenUrlWithManager::openWithUrlInfo() const
{
    return mOpenWithUrlInfo;
}

void OpenUrlWithManager::setOpenWithUrlInfo(const QVector<OpenWithUrlInfo> &newOpenWithUrlInfo)
{
    mOpenWithUrlInfo = newOpenWithUrlInfo;
}
