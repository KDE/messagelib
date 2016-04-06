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

#include "testwebengine.h"
#include <QHBoxLayout>
#include <QApplication>
#include <QWebEngineView>
#include <WebEngineViewer/WebHitTestResult>
#include <WebEngineViewer/WebEnginePage>
#include <QDebug>
#include <QWebEngineSettings>
#include <QContextMenuEvent>
#include <webengine/webhittest.h>

TestWebEngine::TestWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    TestWebEngineView *pageView = new TestWebEngineView(this);
    hboxLayout->addWidget(pageView);
    mEnginePage = new WebEngineViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);
    pageView->setContextMenuPolicy(Qt::DefaultContextMenu);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestWebEngine::~TestWebEngine()
{

}

TestWebEngineView::TestWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{
    setContextMenuPolicy(Qt::DefaultContextMenu);
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
}

void TestWebEngineView::slotHitTestFinished(const WebEngineViewer::WebHitTestResult &result)
{
    qDebug() << " alternateText" << result.alternateText();
    qDebug() << " boundingRect" << result.boundingRect();
    qDebug() << " imageUrl" << result.imageUrl();
    qDebug() << " isContentEditable" << result.isContentEditable();
    qDebug() << " isContentSelected" << result.isContentSelected();
    qDebug() << " isNull" << result.isNull();
    qDebug() << " linkTitle" << result.linkTitle();
    qDebug() << " linkUrl" << result.linkUrl();
    qDebug() << " mediaUrl" << result.mediaUrl();
    qDebug() << " mediaPaused" << result.mediaPaused();
    qDebug() << " mediaMuted" << result.mediaMuted();
    qDebug() << " pos" << result.pos();
    qDebug() << " tagName" << result.tagName();

}

void TestWebEngineView::contextMenuEvent(QContextMenuEvent *e)
{
    WebEngineViewer::WebHitTest *webHit = static_cast<WebEngineViewer::WebEnginePage *>(page())->hitTestContent(e->pos());
    connect(webHit, &WebEngineViewer::WebHitTest::finished, this, &TestWebEngineView::slotHitTestFinished);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestWebEngine *testWebEngine = new TestWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
