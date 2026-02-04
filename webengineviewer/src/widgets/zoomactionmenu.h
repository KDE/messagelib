/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <KActionMenu>
class KActionCollection;
namespace WebEngineViewer
{
class ZoomActionMenuPrivate;
/*!
 * \class WebEngineViewer::ZoomActionMenu
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/ZoomActionMenu
 *
 * \brief The ZoomActionMenu class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT ZoomActionMenu : public KActionMenu
{
    Q_OBJECT
public:
    /*! Constructs a ZoomActionMenu object. */
    explicit ZoomActionMenu(QObject *parent = nullptr);
    /*! Destroys the ZoomActionMenu object. */
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
    /*! Emitted when the zoom factor changes. */
    void zoomChanged(qreal value);

public:
    /*! Zooms in. */
    void slotZoomIn();
    /*! Zooms out. */
    void slotZoomOut();
    /*! Resets the zoom. */
    void slotZoomReset();

private:
    std::unique_ptr<ZoomActionMenuPrivate> const d;
};
}
