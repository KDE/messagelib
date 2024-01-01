/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <memory>
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
    [[nodiscard]] bool isInBackOffMode() const;

    void startOffMode();

    [[nodiscard]] int numberOfHttpFailed() const;

    void slotTimerFinished();

private:
    std::unique_ptr<BackOffModeManagerPrivate> const d;
};
}
