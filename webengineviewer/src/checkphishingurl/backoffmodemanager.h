/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::BackOffModeManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/BackOffModeManager
 *
 * \brief The BackOffModeManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT BackOffModeManager : public QObject
{
public:
    /*! Constructs a BackOffModeManager object. */
    explicit BackOffModeManager(QObject *parent = nullptr);
    /*! Destroys the BackOffModeManager object. */
    ~BackOffModeManager() override;

    /*! Returns the singleton instance. */
    static BackOffModeManager *self();
    /*! Returns whether the manager is in back-off mode. */
    [[nodiscard]] bool isInBackOffMode() const;

    /*! Starts back-off mode. */
    void startOffMode();

    /*! Returns the number of failed HTTP requests. */
    [[nodiscard]] int numberOfHttpFailed() const;

    /*! Handles timer finished event. */
    void slotTimerFinished();

private:
    std::unique_ptr<BackOffModeManagerPrivate> const d;
};
}
