/*
  SPDX-FileCopyrightText: 2021-2023 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include "scamdetectioninfo.h"
#include <QObject>
#include <QVector>
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
    void loadSettings();
    void writeSettings();
    QVector<ScamDetectionInfo> mScamDetectionInfoList;
};
}
