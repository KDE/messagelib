/*
  SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include "scamdetectionwhitelistsettingsmanager.h"

#include <KConfigGroup>
#include <KSharedConfig>

namespace
{
static const char myScamDetectionWhiteListGroupName[] = "ScamDetectionWhiteList";
}
using namespace MessageViewer;
ScamDetectionWhiteListSettingsManager::ScamDetectionWhiteListSettingsManager(QObject *parent)
    : QObject{parent}
{
    loadSettings();
}

ScamDetectionWhiteListSettingsManager::~ScamDetectionWhiteListSettingsManager()
{
    writeSettings();
}

ScamDetectionWhiteListSettingsManager *ScamDetectionWhiteListSettingsManager::self()
{
    static ScamDetectionWhiteListSettingsManager s_self;
    return &s_self;
}

void ScamDetectionWhiteListSettingsManager::loadSettings()
{
    mScamDetectionInfoList.clear();
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myScamDetectionWhiteListGroupName));
    // TODO
}

void ScamDetectionWhiteListSettingsManager::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group(config, QLatin1StringView(myScamDetectionWhiteListGroupName));
    // TODO
}

QList<ScamDetectionInfo> ScamDetectionWhiteListSettingsManager::scamDetectionInfoList() const
{
    return mScamDetectionInfoList;
}

void ScamDetectionWhiteListSettingsManager::setScamDetectionInfoList(const QList<ScamDetectionInfo> &newScamDetectionInfoList)
{
    if (mScamDetectionInfoList != newScamDetectionInfoList) {
        mScamDetectionInfoList = newScamDetectionInfoList;
        writeSettings();
    }
}

#include "moc_scamdetectionwhitelistsettingsmanager.cpp"
