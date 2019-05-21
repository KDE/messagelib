/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#include "testdndwebengine.h"

#include <QApplication>
#include <QVBoxLayout>
#include <webengineview.h>
#include <QTextEdit>
#include <QDebug>

TestDndWebEngine::TestDndWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout;
    vbox->addLayout(layout);
    pageView = new WebEngineViewer::WebEngineView(this);
    pageView->load(QUrl(QStringLiteral("http://www.planetkde.org")));
    layout->addWidget(pageView);

    webEngineView = new WebEngineViewBase(this);
    webEngineView->load(QUrl(QStringLiteral("http://www.kde.org")));
    layout->addWidget(webEngineView);

    QTextEdit *edit = new QTextEdit(this);
    vbox->addWidget(edit);
}

TestDndWebEngine::~TestDndWebEngine()
{
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestDndWebEngine *testWebEngine = new TestDndWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

WebEngineViewBase::WebEngineViewBase(QWidget *parent)
    : QWebEngineView(parent)
{
    qDebug() << "WebEngineViewBase::WebEngineViewBase(QWidget *parent)" << this;
}

void WebEngineViewBase::dragEnterEvent(QDragEnterEvent *e)
{
    qDebug() << " void WebEngineViewBase::dragEnterEvent(QDragEnterEvent *e)";
    QWebEngineView::dragEnterEvent(e);
}

void WebEngineViewBase::dragLeaveEvent(QDragLeaveEvent *e)
{
    qDebug() << " void WebEngineViewBase::dragLeaveEvent(QDragEnterEvent *e)";
    QWebEngineView::dragLeaveEvent(e);
}

void WebEngineViewBase::dragMoveEvent(QDragMoveEvent *e)
{
    qDebug() << "void WebEngineViewBase::dragMoveEvent(QDragMoveEvent *e)";
    QWebEngineView::dragMoveEvent(e);
}

void WebEngineViewBase::dropEvent(QDropEvent *e)
{
    qDebug() << " void WebEngineViewBase::dropEvent(QDropEvent *e)";
    QWebEngineView::dropEvent(e);
}
