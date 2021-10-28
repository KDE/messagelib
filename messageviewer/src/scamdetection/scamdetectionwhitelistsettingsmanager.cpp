/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#include "scamdetectionwhitelistsettingsmanager.h"

using namespace MessageViewer;
ScamDetectionWhiteListSettingsManager::ScamDetectionWhiteListSettingsManager(QObject *parent)
    : QObject{parent}
{
}

ScamDetectionWhiteListSettingsManager::~ScamDetectionWhiteListSettingsManager()
{
}

ScamDetectionWhiteListSettingsManager *ScamDetectionWhiteListSettingsManager::self()
{
    static ScamDetectionWhiteListSettingsManager s_self;
    return &s_self;
}
