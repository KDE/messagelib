/*
  SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include "scamdetectioninfo.h"
#include <QList>
#include <QObject>
namespace MessageViewer
{
class ScamDetectionInfo;
class MESSAGEVIEWER_EXPORT ScamDetectionWhiteListSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWhiteListSettingsManager(QObject *parent = nullptr);
    ~ScamDetectionWhiteListSettingsManager() override;
    static ScamDetectionWhiteListSettingsManager *self();

private:
    MESSAGEVIEWER_NO_EXPORT void loadSettings();
    MESSAGEVIEWER_NO_EXPORT void writeSettings();
    QList<ScamDetectionInfo> mScamDetectionInfoList;
};
}
