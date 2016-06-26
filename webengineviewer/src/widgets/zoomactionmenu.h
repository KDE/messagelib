/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef WEBENGINEZOOMACTIONMENU_H
#define WEBENGINEZOOMACTIONMENU_H

#include <KActionMenu>
#include "webengineviewer_export.h"
class KActionCollection;
namespace WebEngineViewer
{
class ZoomActionMenuPrivate;
class WEBENGINEVIEWER_EXPORT ZoomActionMenu : public KActionMenu
{
    Q_OBJECT
public:
    explicit ZoomActionMenu(QObject *parent = Q_NULLPTR);
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
