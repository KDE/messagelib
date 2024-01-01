/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "zoomactionmenu.h"
#include <KActionCollection>
#include <KLocalizedString>

using namespace WebEngineViewer;
namespace
{
constexpr qreal zoomBy()
{
    return 20;
}
}

class WebEngineViewer::ZoomActionMenuPrivate
{
public:
    explicit ZoomActionMenuPrivate(KActionMenu *qq)
        : q(qq)
    {
    }

    void createMenu();
    qreal mZoomFactor = 100;
    QAction *mZoomInAction = nullptr;
    QAction *mZoomOutAction = nullptr;
    QAction *mZoomResetAction = nullptr;
    KActionCollection *mActionCollection = nullptr;
    KActionMenu *const q;
};

ZoomActionMenu::ZoomActionMenu(QObject *parent)
    : KActionMenu(parent)
    , d(new WebEngineViewer::ZoomActionMenuPrivate(this))
{
}

ZoomActionMenu::~ZoomActionMenu() = default;

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

    d->mZoomResetAction = KStandardAction::actualSize(this, &ZoomActionMenu::slotZoomReset, this);
    d->mActionCollection->addAction(QStringLiteral("zoom_reset"), d->mZoomResetAction);

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

#include "moc_zoomactionmenu.cpp"
