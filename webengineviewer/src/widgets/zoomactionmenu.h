/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] QAction *zoomInAction() const;

    [[nodiscard]] QAction *zoomOutAction() const;

    [[nodiscard]] QAction *zoomResetAction() const;

    void setActionCollection(KActionCollection *ac);

    void setZoomFactor(qreal zoomFactor);
    [[nodiscard]] qreal zoomFactor() const;

    void setWebViewerZoomFactor(qreal zoomFactor);

Q_SIGNALS:
    void zoomChanged(qreal value);

public Q_SLOTS:
    void slotZoomIn();
    void slotZoomOut();
    void slotZoomReset();

private:
    std::unique_ptr<ZoomActionMenuPrivate> const d;
};
}
