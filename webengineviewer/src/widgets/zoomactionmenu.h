/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WEBENGINEZOOMACTIONMENU_H
#define WEBENGINEZOOMACTIONMENU_H

#include <KActionMenu>
#include "webengineviewer_export.h"
class KActionCollection;
namespace WebEngineViewer {
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
    ~ZoomActionMenu();

    void createZoomActions();

    QAction *zoomInAction() const;

    QAction *zoomOutAction() const;

    QAction *zoomResetAction() const;

    void setActionCollection(KActionCollection *ac);

    void setZoomFactor(qreal zoomFactor);
    qreal zoomFactor() const;

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

#endif // ZOOMACTIONMENU_H
