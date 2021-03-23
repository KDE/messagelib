/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
namespace WebEngineViewer
{
class BackOffModeManagerPrivate;
// https://developers.google.com/safe-browsing/v4/request-frequency
/**
 * @brief The BackOffModeManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT BackOffModeManager : public QObject
{
public:
    explicit BackOffModeManager(QObject *parent = nullptr);
    ~BackOffModeManager() override;

    static BackOffModeManager *self();
    bool isInBackOffMode() const;

    void startOffMode();

    int numberOfHttpFailed() const;

public Q_SLOTS:
    void slotTimerFinished();

private:
    BackOffModeManagerPrivate *const d;
};
}

