/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

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
class WEBENGINEVIEWER_EXPORT TrackingWarningWidget : public KMessageWidget
{
    Q_OBJECT
public:
    explicit TrackingWarningWidget(QWidget *parent = nullptr);
    ~TrackingWarningWidget() override;
    void addTracker(const WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList &);
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
