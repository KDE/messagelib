/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "printpreviewpageviewer.h"
#include <poppler-qt5.h>

#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QWheelEvent>
#include <QShortcut>

using namespace WebEngineViewer;

PrintPreviewPageViewer::PrintPreviewPageViewer(QWidget *parent)
    : QScrollArea(parent),
      mDpiX(QApplication::desktop()->physicalDpiX()),
      mDpiY(QApplication::desktop()->physicalDpiY()),
      mZoom(1.0)
{
    mImage = new QLabel(this);
    mImage->setObjectName(QStringLiteral("page"));
    mImage->resize(0, 0);
    setWidget(mImage);
    initializeActions();
}

PrintPreviewPageViewer::~PrintPreviewPageViewer()
{

}

void PrintPreviewPageViewer::initializeActions()
{
    QShortcut *shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(Qt::Key_PageUp));
    connect(shortcut, &QShortcut::activated, this, &PrintPreviewPageViewer::slotPageUp);

    shortcut = new QShortcut(this);
    shortcut->setKey(QKeySequence(Qt::Key_PageDown));
    connect(shortcut, &QShortcut::activated, this, &PrintPreviewPageViewer::slotPageDown);
}

void PrintPreviewPageViewer::slotPageUp()
{
    //TODO
}

void PrintPreviewPageViewer::slotPageDown()
{
    //TODO
}

void PrintPreviewPageViewer::showPage(Poppler::Page *page)
{
    const double resX = mDpiX * mZoom;
    const double resY = mDpiY * mZoom;

    const QImage image = page->renderToImage(resX, resY);
    if (!image.isNull()) {
        mImage->resize(image.size());
        mImage->setPixmap(QPixmap::fromImage(image));
    } else {
        mImage->resize(0, 0);
        mImage->setPixmap(QPixmap());
    }
}

void PrintPreviewPageViewer::setZoom(qreal zoom)
{
    if (mZoom != zoom) {
        mZoom = zoom;
        Q_EMIT reloadPage();
    }
}

qreal PrintPreviewPageViewer::zoom() const
{
    return mZoom;
}

void PrintPreviewPageViewer::wheelEvent(QWheelEvent *e)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        const int numDegrees = e->delta() / 8;
        const int numSteps = numDegrees / 15;

        const qreal factor = (mZoom * 100) + numSteps * 10;
        if (factor >= 10 && factor <= 300) {
            setZoom(factor / 100);
        }
        e->accept();
        return;
    }
    QScrollArea::wheelEvent(e);
}
