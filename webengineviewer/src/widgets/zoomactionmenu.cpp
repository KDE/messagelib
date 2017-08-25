/*
   Copyright (c) 2015-2017 Laurent Montel <montel@kde.org>

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

#include "zoomactionmenu.h"
#include <KLocalizedString>
#include <KActionCollection>

using namespace WebEngineViewer;
namespace
{
qreal zoomBy()
{
    return 20;
}
}

class WebEngineViewer::ZoomActionMenuPrivate
{
public:
    ZoomActionMenuPrivate(KActionMenu *qq)
        : q(qq)
    {

    }
    void createMenu();
    qreal mZoomFactor = 100;
    QAction *mZoomInAction = nullptr;
    QAction *mZoomOutAction = nullptr;
    QAction *mZoomResetAction = nullptr;
    KActionCollection *mActionCollection = nullptr;
    KActionMenu *q = nullptr;
};

ZoomActionMenu::ZoomActionMenu(QObject *parent)
    : KActionMenu(parent),
      d(new WebEngineViewer::ZoomActionMenuPrivate(this))
{
}

ZoomActionMenu::~ZoomActionMenu()
{
    delete d;
}

void ZoomActionMenu::setActionCollection(KActionCollection *ac)
{
    d->mActionCollection = ac;
}

void ZoomActionMenu::createZoomActions()
{
    // Zoom actions
    d->mZoomInAction = new QAction(QIcon::fromTheme(QStringLiteral("zoom-in")), i18n("&Zoom In"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_in"), d->mZoomInAction);
    connect(d->mZoomInAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomIn);
    d->mActionCollection->setDefaultShortcut(d->mZoomInAction, QKeySequence(Qt::CTRL + Qt::Key_Plus));

    d->mZoomOutAction = new QAction(QIcon::fromTheme(QStringLiteral("zoom-out")), i18n("Zoom &Out"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_out"), d->mZoomOutAction);
    connect(d->mZoomOutAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomOut);
    d->mActionCollection->setDefaultShortcut(d->mZoomOutAction, QKeySequence(Qt::CTRL + Qt::Key_Minus));

    d->mZoomResetAction = new QAction(i18n("Reset"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_reset"), d->mZoomResetAction);
    connect(d->mZoomResetAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomReset);
    d->mActionCollection->setDefaultShortcut(d->mZoomResetAction, QKeySequence(Qt::CTRL + Qt::Key_0));
    d->createMenu();
}

QAction *ZoomActionMenu::zoomInAction() const
{
    return d->mZoomInAction;
}

QAction *ZoomActionMenu::zoomOutAction() const
{
    return d->mZoomOutAction;
}

QAction *ZoomActionMenu::zoomResetAction() const
{
    return d->mZoomResetAction;
}

void ZoomActionMenu::setZoomFactor(qreal zoomFactor)
{
    d->mZoomFactor = zoomFactor;
}

void ZoomActionMenu::setWebViewerZoomFactor(qreal zoomFactor)
{
    Q_EMIT zoomChanged(zoomFactor);
}

void ZoomActionMenu::slotZoomIn()
{
    if (d->mZoomFactor >= 300) {
        return;
    }
    d->mZoomFactor += zoomBy();
    if (d->mZoomFactor > 300) {
        d->mZoomFactor = 300;
    }
    Q_EMIT zoomChanged(d->mZoomFactor / 100.0);
}

void ZoomActionMenu::slotZoomOut()
{
    if (d->mZoomFactor <= 10) {
        return;
    }
    d->mZoomFactor -= zoomBy();
    if (d->mZoomFactor < 10) {
        d->mZoomFactor = 10;
    }
    Q_EMIT zoomChanged(d->mZoomFactor / 100.0);
}

void ZoomActionMenu::slotZoomReset()
{
    d->mZoomFactor = 100;
    Q_EMIT zoomChanged(1.0);
}

qreal ZoomActionMenu::zoomFactor() const
{
    return d->mZoomFactor;
}

void ZoomActionMenuPrivate::createMenu()
{
    q->setText(i18n("Zoom"));
    q->addAction(mZoomInAction);
    q->addAction(mZoomOutAction);
    q->addSeparator();
    q->addAction(mZoomResetAction);
    mActionCollection->addAction(QStringLiteral("zoom_menu"), q);
}
