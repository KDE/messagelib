/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

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

#include "printpreviewpageviewer.h"
#include <poppler-qt5.h>

#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>

using namespace WebEngineViewer;

PrintPreviewPageViewer::PrintPreviewPageViewer(QWidget *parent)
    : QScrollArea(parent),
      mDpiX(QApplication::desktop()->physicalDpiX()),
      mDpiY(QApplication::desktop()->physicalDpiY())
{

    mImage = new QLabel(this);
    mImage->setObjectName(QStringLiteral("page"));
    mImage->resize(0, 0);
    setWidget(mImage);
}

PrintPreviewPageViewer::~PrintPreviewPageViewer()
{

}

void PrintPreviewPageViewer::showPage(Poppler::Page *page)
{
    const QImage image = page->renderToImage(mDpiX, mDpiY);
    if (!image.isNull()) {
        mImage->resize(image.size());
        mImage->setPixmap(QPixmap::fromImage(image));
    } else {
        mImage->resize(0, 0);
        mImage->setPixmap(QPixmap());
    }
}
