/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testwebengine.h"
#include "webengineviewer/webhittest.h"
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QHBoxLayout>
#include <QWebEngineSettings>
#include <WebEngineViewer/WebEnginePage>
#include <WebEngineViewer/WebHitTestResult>

TestWebEngine::TestWebEngine(QWidget *parent)
    : QWidget(parent)
{
    auto hboxLayout = new QHBoxLayout(this);
    auto pageView = new TestWebEngineView(this);
    hboxLayout->addWidget(pageView);
    mEnginePage = new WebEngineViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);
    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestWebEngine::~TestWebEngine() = default;

TestWebEngineView::TestWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{
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
    auto testWebEngine = new TestWebEngine;
    testWebEngine->show();
    testWebEngine->resize(600, 400);
    const int ret = app.exec();
    return ret;
}

#include "moc_testwebengine.cpp"
