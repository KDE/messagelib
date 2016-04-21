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
#include "testmailwebengineselection.h"
#include "webenginescript.h"

#include <KActionCollection>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineSettings>
#include <QMessageBox>

#include <MessageViewer/MailWebEngineView>

TestMailWebEngineSelection::TestMailWebEngineSelection(QWidget *parent)
    : QWidget(parent),
      mNumber(0)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    connect(mTestWebEngine, &MessageViewer::MailWebEngineView::openUrl, this, &TestMailWebEngineSelection::slotOpenUrl);
    QString str = QStringLiteral("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n  <style type=\"text/css\">\n  /*<![CDATA[*/\n    @import \"main.css\";\n  /*]]>*/\n\n.links {\n    margin: auto;\n}\n\n.links td {\n    padding-top: 5px;\n    padding-bottom: 5px;\n}\n\n  </style>\n\n  <title>Akregator</title>\n</head>\n\n<body>\n  <div id=\"header\"><img src=\"file:///opt/kde5/share/icons/maia/apps/scalable/akregator.svg\" align=\"top\" height=\"128\" width=\"128\" alt=\"akregator\" title=\"\" />\n    <div id=\"title\">\n        <h1>Akregator</h1>Akregator est un agrégateur de flux pour KDE.\n    </div>\n  </div>\n\n  <div id=\"box\">\n    <div id=\"boxInner\">\n\n<div class=\"center\">\n    <p>Feed readers provide a convenient way to browse different kinds of content, including news, blogs, and other content from online sites. Instead of checking all your favorite web sites manually for updates, Akregator collects the content for you.</p>\n    <p> For more information about using Akregator, check the <a href='http://akregator.kde.org/'>Akregator website</a>. If you do not want to see this page anymore, <a href='config:/disable_introduction'>click here</a>.</p>\n    <p>We hope that you will enjoy Akregator.</p>\n    <p>Thank you, The Akregator Team </p>\n</div>\n\n    </div>\n  </div>\n</body>\n</html>\n\n<!-- vim:set sw=2 et nocindent smartindent: -->\n");
    mTestWebEngine->setHtml(str, QUrl(QStringLiteral("file:///")));
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vbox->addWidget(mTestWebEngine);
    QHBoxLayout *hButtonBox = new QHBoxLayout;
    vbox->addLayout(hButtonBox);

    QPushButton *changeHtml = new QPushButton(QStringLiteral("switch html"), this);
    connect(changeHtml, &QPushButton::clicked, this, &TestMailWebEngineSelection::slotSwitchHtml);
    hButtonBox->addWidget(changeHtml);

    QPushButton *showSelection = new QPushButton(QStringLiteral("Show Selection"), this);
    connect(showSelection, &QPushButton::clicked, this, &TestMailWebEngineSelection::slotShowSelection);
    hButtonBox->addWidget(showSelection);
}

TestMailWebEngineSelection::~TestMailWebEngineSelection()
{

}

void TestMailWebEngineSelection::slotSwitchHtml()
{
    QString str = QStringLiteral("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head>\n  <style type=\"text/css\">\n  /*<![CDATA[*/\n    @import \"main.css\";\n  /*]]>*/\n\n.links {\n    margin: auto;\n}\n\n.links td {\n    padding-top: 5px;\n    padding-bottom: 5px;\n}\n\n  </style>\n\n  <title>Akregator</title>\n</head>\n\n<body>\n  <div id=\"header\"><img src=\"file:///opt/kde5/share/icons/maia/apps/scalable/akregator.svg\" align=\"top\" height=\"128\" width=\"128\" alt=\"akregator\" title=\"\" />\n    <div id=\"title\">\n        <h1>Akregator</h1>Akregator est un agrégateur de flux pour KDE.\n    </div>\n  </div>\n\n  <div id=\"box\">\n    <div id=\"boxInner\">\n\n<div class=\"center\">\n    <p>Feed readers provide a convenient way to browse different kinds of content, including news, blogs, and other content from online sites. Instead of checking all your favorite web sites manually for updates, Akregator collects the content for you.</p>\n    <p> For more information about using Akregator, check the <a href='http://akregator.kde.org/'>Akregator website</a>. If you do not want to see this page anymore, <a href='config:/disable_introduction'>click here</a>.</p>\n    <p>We hope that you will enjoy Akregator.</p>\n    <p>Thank you, number %1 </p>\n</div>\n\n    </div>\n  </div>\n</body>\n</html>\n\n<!-- vim:set sw=2 et nocindent smartindent: -->\n").arg(mNumber);
    mTestWebEngine->setHtml(str, QUrl(QStringLiteral("file:///")));
    mNumber++;
}

void TestMailWebEngineSelection::slotShowSelection()
{
    QMessageBox::information(this, QStringLiteral("selection"), mTestWebEngine->selectedText());
}

void TestMailWebEngineSelection::slotOpenUrl(const QUrl &url)
{
    mTestWebEngine->load(url);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestMailWebEngineSelection *testWebEngine = new TestMailWebEngineSelection;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
