/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMessageWidget>

#include "webengineviewer_export.h"

#include <QMap>
#include <QPointer>

#include <WebEngineViewer/BlockTrackingUrlInterceptor>

namespace WebEngineViewer
{
class TrackingDetailsDialog;
/*!
 * \class WebEngineViewer::TrackingWarningWidget
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/TrackingWarningWidget
 *
 * \brief The TrackingWarningWidget class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT TrackingWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    /*! Constructs a TrackingWarningWidget with the given parent. */
    explicit TrackingWarningWidget(QWidget *parent = nullptr);
    /*! Destroys the TrackingWarningWidget. */
    ~TrackingWarningWidget() override;
    /*! Adds a tracker to the list. */
    void addTracker(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);
    /*! Hides the widget and clears the tracking data. */
    void hideAndClear();

private:
    WEBENGINEVIEWER_NO_EXPORT void slotShowDetails(const QString &content);
    [[nodiscard]] WEBENGINEVIEWER_NO_EXPORT QString generateDetails() const;

    struct blackListFound {
        QString url;
        int number = 1;
    };
    QMap<QString, blackListFound> mBackLists;
    QPointer<TrackingDetailsDialog> mMailTrackingDetailDialog;
};
}
