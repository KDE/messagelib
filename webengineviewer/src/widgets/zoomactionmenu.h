/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <KActionMenu>
class KActionCollection;
namespace WebEngineViewer
{
class ZoomActionMenuPrivate;
/**
 * @brief The ZoomActionMenu class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT ZoomActionMenu : public KActionMenu
{
    Q_OBJECT
public:
    explicit ZoomActionMenu(QObject *parent = nullptr);
    ~ZoomActionMenu() override;

    void createZoomActions();

    QAction *zoomInAction() const;

    QAction *zoomOutAction() const;

    QAction *zoomResetAction() const;

    void setActionCollection(KActionCollection *ac);

    void setZoomFactor(qreal zoomFactor);
    Q_REQUIRED_RESULT qreal zoomFactor() const;

    void setWebViewerZoomFactor(qreal zoomFactor);

Q_SIGNALS:
    void zoomChanged(qreal value);

public Q_SLOTS:
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomReset();

private:
    ZoomActionMenuPrivate *const d;
};
}

