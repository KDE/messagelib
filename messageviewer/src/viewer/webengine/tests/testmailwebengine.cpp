/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testmailwebengine.h"
#include "viewer/printmessage.h"
#include "webenginescript.h"

#include <KActionCollection>
#include <QApplication>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineSettings>

#include <MessageViewer/MailWebEngineView>
#include <WebEngineViewer/WebEngineManageScript>

TestMailWebEngine::TestMailWebEngine(QWidget *parent)
    : QWidget(parent)
    , mZoom(1.0)
{
    auto vbox = new QVBoxLayout(this);
    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    connect(mTestWebEngine, &MessageViewer::MailWebEngineView::openUrl, this, &TestMailWebEngine::slotOpenUrl);
    // mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
    QString str = QStringLiteral(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
        "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n  <style type=\"text/css\">\n  "
        "/*<![CDATA[*/\n    @import \"main.css\";\n  /*]]>*/\n\n.links {\n    margin: auto;\n}\n\n.links td {\n    padding-top: 5px;\n    padding-bottom: "
        "5px;\n}\n\n  </style>\n\n  <title>Akregator</title>\n</head>\n\n<body>\n  <div id=\"header\"><img "
        "src=\"file:///opt/kde5/share/icons/maia/apps/scalable/akregator.svg\" align=\"top\" height=\"128\" width=\"128\" alt=\"akregator\" title=\"\" />\n    "
        "<div id=\"title\">\n        <h1>Akregator</h1>Akregator est un agrégateur de flux pour KDE.\n    </div>\n  </div>\n\n  <div id=\"box\">\n    <div "
        "id=\"boxInner\">\n\n<div class=\"center\">\n    <p>Feed readers provide a convenient way to browse different kinds of content, including news, blogs, "
        "and other content from online sites. Instead of checking all your favorite web sites manually for updates, Akregator collects the content for "
        "you.</p>\n    <p> For more information about using Akregator, check the <a href='http://akregator.kde.org/'>Akregator website</a>. If you do not want "
        "to see this page anymore, <a href='config:/disable_introduction'>click here</a>.</p>\n    <p>We hope that you will enjoy Akregator.</p>\n    <p>Thank "
        "you, The Akregator Team </p>\n</div>\n\n    </div>\n  </div>\n</body>\n</html>\n\n<!-- vim:set sw=2 et nocindent smartindent: -->\n");
    mTestWebEngine->setHtml(str, QUrl(QStringLiteral("file:///")));
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vbox->addWidget(mTestWebEngine);
    auto hButtonBox = new QHBoxLayout;
    vbox->addLayout(hButtonBox);

    auto scrollUp = new QPushButton(QStringLiteral("scrollUp 10px"), this);
    connect(scrollUp, &QPushButton::clicked, this, &TestMailWebEngine::slotScrollUp);
    hButtonBox->addWidget(scrollUp);

    auto scrollDown = new QPushButton(QStringLiteral("scrollDown 10px"), this);
    connect(scrollDown, &QPushButton::clicked, this, &TestMailWebEngine::slotScrollDown);
    hButtonBox->addWidget(scrollDown);

    hButtonBox = new QHBoxLayout;
    vbox->addLayout(hButtonBox);

    auto zoomUp = new QPushButton(QStringLiteral("zoom Up"), this);
    connect(zoomUp, &QPushButton::clicked, this, &TestMailWebEngine::slotZoomUp);
    hButtonBox->addWidget(zoomUp);

    auto zoomDown = new QPushButton(QStringLiteral("zoom Down"), this);
    connect(zoomDown, &QPushButton::clicked, this, &TestMailWebEngine::slotZoomDown);
    hButtonBox->addWidget(zoomDown);

    auto printPreview = new QPushButton(QStringLiteral("Print Preview"), this);
    connect(printPreview, &QPushButton::clicked, this, &TestMailWebEngine::slotPrintPreview);
    hButtonBox->addWidget(printPreview);
}

TestMailWebEngine::~TestMailWebEngine() = default;

void TestMailWebEngine::slotOpenUrl(const QUrl &url)
{
    mTestWebEngine->load(url);
}

void TestMailWebEngine::slotScrollDown()
{
    mTestWebEngine->page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollDown(10), WebEngineViewer::WebEngineManageScript::scriptWordId());
}

void TestMailWebEngine::slotScrollUp()
{
    mTestWebEngine->page()->runJavaScript(WebEngineViewer::WebEngineScript::scrollUp(10), WebEngineViewer::WebEngineManageScript::scriptWordId());
}

void TestMailWebEngine::slotZoomDown()
{
    mZoom -= 0.2;
    mTestWebEngine->setZoomFactor(mZoom);
}

void TestMailWebEngine::slotZoomUp()
{
    mZoom += 0.2;
    mTestWebEngine->setZoomFactor(mZoom);
}

void TestMailWebEngine::slotPrintPreview()
{
    auto printMessage = new MessageViewer::PrintMessage(this);
    printMessage->setView(mTestWebEngine);
    printMessage->setParentWidget(this);
    printMessage->printPreview();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto testWebEngine = new TestMailWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

#include "moc_testmailwebengine.cpp"
