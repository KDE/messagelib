/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "zoomactionmenu.h"
#include <KLocalizedString>
#include <KToggleAction>
#include <KActionCollection>
#include <viewer/mailwebview.h>

using namespace MessageViewer;
namespace
{
qreal zoomBy()
{
    return 20;
}
}

class MessageViewer::ZoomActionMenuPrivate
{
public:
    ZoomActionMenuPrivate()
        : mZoomFactor(100),
          mZoomTextOnlyAction(Q_NULLPTR),
          mZoomInAction(Q_NULLPTR),
          mZoomOutAction(Q_NULLPTR),
          mZoomResetAction(Q_NULLPTR),
          mActionCollection(Q_NULLPTR),
          mZoomTextOnly(false)
    {

    }
    qreal mZoomFactor;
    KToggleAction *mZoomTextOnlyAction;
    QAction *mZoomInAction;
    QAction *mZoomOutAction;
    QAction *mZoomResetAction;
    KActionCollection *mActionCollection;
    bool mZoomTextOnly;
};

ZoomActionMenu::ZoomActionMenu(QObject *parent)
    : KActionMenu(parent),
      d(new MessageViewer::ZoomActionMenuPrivate())
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
    d->mZoomTextOnlyAction = new KToggleAction(i18n("Zoom Text Only"), this);
    d->mActionCollection->addAction(QStringLiteral("toggle_zoomtextonly"), d->mZoomTextOnlyAction);
    connect(d->mZoomTextOnlyAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomTextOnly);

    d->mZoomInAction = new QAction(QIcon::fromTheme(QStringLiteral("zoom-in")), i18n("&Zoom In"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_in"), d->mZoomInAction);
    connect(d->mZoomInAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomIn);
    d->mActionCollection->setDefaultShortcut(d->mZoomInAction, QKeySequence(Qt::CTRL | Qt::Key_Plus));

    d->mZoomOutAction = new QAction(QIcon::fromTheme(QStringLiteral("zoom-out")), i18n("Zoom &Out"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_out"), d->mZoomOutAction);
    connect(d->mZoomOutAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomOut);
    d->mActionCollection->setDefaultShortcut(d->mZoomOutAction, QKeySequence(Qt::CTRL | Qt::Key_Minus));

    d->mZoomResetAction = new QAction(i18n("Reset"), this);
    d->mActionCollection->addAction(QStringLiteral("zoom_reset"), d->mZoomResetAction);
    connect(d->mZoomResetAction, &QAction::triggered, this, &ZoomActionMenu::slotZoomReset);
    d->mActionCollection->setDefaultShortcut(d->mZoomResetAction, QKeySequence(Qt::CTRL | Qt::Key_0));

}

KToggleAction *ZoomActionMenu::zoomTextOnlyAction() const
{
    return d->mZoomTextOnlyAction;
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

void ZoomActionMenu::setZoomTextOnly(bool textOnly)
{
    d->mZoomTextOnly = textOnly;
    if (d->mZoomTextOnlyAction) {
        d->mZoomTextOnlyAction->setChecked(d->mZoomTextOnly);
    }
    Q_EMIT zoomTextOnlyChanged(d->mZoomTextOnly);
}

void ZoomActionMenu::slotZoomTextOnly()
{
    setZoomTextOnly(!d->mZoomTextOnly);
}

void ZoomActionMenu::slotZoomReset()
{
    d->mZoomFactor = 100;
    Q_EMIT zoomChanged(1.0);
}

bool ZoomActionMenu::zoomTextOnly() const
{
    return d->mZoomTextOnly;
}

qreal ZoomActionMenu::zoomFactor() const
{
    return d->mZoomFactor;
}
