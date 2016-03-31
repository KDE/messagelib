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
#include "testmailwebengine.h"
#include "webengine/webenginescript.h"

#include <KActionCollection>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineSettings>

#include <viewer/webengine/mailwebengineview.h>

TestMailWebEngine::TestMailWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vbox->addWidget(mTestWebEngine);
    QHBoxLayout *hButtonBox = new QHBoxLayout;
    vbox->addLayout(hButtonBox);

    QPushButton *scrollUp = new QPushButton(QStringLiteral("scrollUp 10px"), this);
    connect(scrollUp, &QPushButton::clicked, this, &TestMailWebEngine::slotScrollUp);
    hButtonBox->addWidget(scrollUp);

    QPushButton *scrollDown = new QPushButton(QStringLiteral("scrollDown 10px"), this);
    connect(scrollDown, &QPushButton::clicked, this, &TestMailWebEngine::slotScrollDown);
    hButtonBox->addWidget(scrollDown);
}

TestMailWebEngine::~TestMailWebEngine()
{

}

void TestMailWebEngine::slotScrollDown()
{
    mTestWebEngine->page()->runJavaScript(MessageViewer::WebEngineScript::scrollDown(10));
}

void TestMailWebEngine::slotScrollUp()
{
    mTestWebEngine->page()->runJavaScript(MessageViewer::WebEngineScript::scrollUp(10));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestMailWebEngine *testWebEngine = new TestMailWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
