/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentmanager.h"
#include "remotecontentinfo.h"
#include <KConfigGroup>
#include <KSharedConfig>

using namespace MessageViewer;
namespace
{
static const char myRemoteContentGroupName[] = "RemoteContent";
}
RemoteContentManager::RemoteContentManager(QObject *parent)
    : QObject(parent)
{
    loadSettings();
}

RemoteContentManager::~RemoteContentManager()
{
    writeSettings();
}

RemoteContentManager *RemoteContentManager::self()
{
    static RemoteContentManager s_self;
    return &s_self;
}

void RemoteContentManager::clear()
{
    mRemoveContentInfo.clear();
}

bool RemoteContentManager::isAutorized(const QString &url) const
{
    for (const RemoteContentInfo &info : std::as_const(mRemoveContentInfo)) {
        if (info.url() == url) {
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized;
        }
    }
    return false;
}

bool RemoteContentManager::isAutorized(const QUrl &url, bool &contains) const
{
    const QString host = url.host();
    const QString urlToString = url.toString();

    contains = false;

    for (const RemoteContentInfo &info : std::as_const(mRemoveContentInfo)) {
        if (info.url() == urlToString) {
            contains = true;
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized;
        } else if (info.url() == host) {
            contains = true;
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized;
        }
    }
    return false;
}

bool RemoteContentManager::isBlocked(const QUrl &url, bool &contains) const
{
    const QString host = url.host();
    const QString urlToString = url.toString();

    contains = false;

    for (const RemoteContentInfo &info : std::as_const(mRemoveContentInfo)) {
        if (info.url() == urlToString) {
            contains = true;
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Blocked;
        } else if (info.url() == host) {
            contains = true;
            return info.status() == RemoteContentInfo::RemoteContentInfoStatus::Blocked;
        }
    }
    return false;
}

void RemoteContentManager::loadSettings()
{
    mRemoveContentInfo.clear();
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myRemoteContentGroupName));
    const QStringList blockedUrl = group.readEntry("Blocked", QStringList());
    const QStringList authorizedUrl = group.readEntry("Authorized", QStringList());
    mRemoveContentInfo.reserve(blockedUrl.count() + authorizedUrl.count());
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
}

void RemoteContentManager::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myRemoteContentGroupName));
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
}

void RemoteContentManager::setRemoveContentInfo(const QList<RemoteContentInfo> &removeContentInfo)
{
    mRemoveContentInfo = removeContentInfo;
}

QList<RemoteContentInfo> RemoteContentManager::removeContentInfo() const
{
    return mRemoveContentInfo;
}

void RemoteContentManager::addRemoteContent(const RemoteContentInfo &info)
{
    mRemoveContentInfo.append(info);
}

bool RemoteContentManager::isUnique(const RemoteContentInfo &newInfo) const
{
    for (const RemoteContentInfo &info : std::as_const(mRemoveContentInfo)) {
        if (info.url() == newInfo.url()) {
            return false;
        }
    }
    return true;
}

#include "moc_remotecontentmanager.cpp"
