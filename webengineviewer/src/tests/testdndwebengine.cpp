/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testdndwebengine.h"

#include <QApplication>
#include <QDebug>
#include <QTextEdit>
#include <QVBoxLayout>
#include <webengineview.h>

TestDndWebEngine::TestDndWebEngine(QWidget *parent)
    : QWidget(parent)
{
    auto vbox = new QVBoxLayout(this);
    auto layout = new QHBoxLayout;
    vbox->addLayout(layout);
    pageView = new WebEngineViewer::WebEngineView(this);
    pageView->load(QUrl(QStringLiteral("http://www.planetkde.org")));
    layout->addWidget(pageView);

    webEngineView = new WebEngineViewBase(this);
    webEngineView->load(QUrl(QStringLiteral("http://www.kde.org")));
    layout->addWidget(webEngineView);

    auto edit = new QTextEdit(this);
    vbox->addWidget(edit);
}

TestDndWebEngine::~TestDndWebEngine()
{
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    auto testWebEngine = new TestDndWebEngine;
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
