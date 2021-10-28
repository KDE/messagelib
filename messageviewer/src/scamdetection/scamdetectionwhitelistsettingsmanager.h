/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer_export.h"
#include <QObject>
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT ScamDetectionWhiteListSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit ScamDetectionWhiteListSettingsManager(QObject *parent = nullptr);
    ~ScamDetectionWhiteListSettingsManager() override;
    static ScamDetectionWhiteListSettingsManager *self();
};
}
