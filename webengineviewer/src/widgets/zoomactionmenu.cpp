/*
   Copyright (C) 2015-2018 Laurent Montel <montel@kde.org>

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
#include <kconfig_version.h>

using namespace WebEngineViewer;
namespace {
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
    : KActionMenu(parent)
    , d(new WebEngineViewer::ZoomActionMenuPrivate(this))
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
    d->mZoomInAction = KStandardAction::zoomIn(this, &ZoomActionMenu::slotZoomIn, this);
    d->mActionCollection->addAction(QStringLiteral("zoom_in"), d->mZoomInAction);

    d->mZoomOutAction = KStandardAction::zoomOut(this, &ZoomActionMenu::slotZoomOut, this);
    d->mActionCollection->addAction(QStringLiteral("zoom_out"), d->mZoomOutAction);

#if KCONFIG_VERSION < QT_VERSION_CHECK(5, 50, 0)
    d->mZoomResetAction = new QAction(i18n("Reset"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_reset"), d->mZoomResetAction);
    connect(d->mZoomResetAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomReset);
    d->mActionCollection->setDefaultShortcut(d->mZoomResetAction, QKeySequence(Qt::CTRL + Qt::Key_0));
#else
    d->mZoomResetAction = KStandardAction::actualSize(this, &ZoomActionMenu::slotZoomReset, this);
    d->mActionCollection->addAction(QStringLiteral("zoom_reset"), d->mZoomResetAction);
#endif

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
