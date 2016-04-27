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

#include "testwebengineaccesskey.h"
#include "../mailwebengineview.h"

#include "messageviewer/messageviewersettings.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QWebEngineSettings>
#include <QStandardPaths>

#include <QLabel>
#include <QPushButton>

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
{
    WebEngineViewer::MessageViewerSettings::self()->setAccessKeyEnabled(true);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->setMargin(0);
    TestWebEngineAccesskey *webEngine = new TestWebEngineAccesskey(this);
    hbox->addWidget(webEngine);

    TestWebKitAccesskey *webKit = new TestWebKitAccesskey(this);
    hbox->addWidget(webKit);
}

TestWidget::~TestWidget()
{

}

TestWebEngineAccesskey::TestWebEngineAccesskey(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    QLabel *label = new QLabel(QStringLiteral("WebEngine"));
    vboxLayout->addWidget(label);

    mTestWebEngine = new WebEngineViewer::MailWebEngineView(new KActionCollection(this), this);
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
    QPushButton *searchAccessKey = new QPushButton(QStringLiteral("AccessKey"), this);
    vboxLayout->addWidget(searchAccessKey);
    connect(searchAccessKey, &QPushButton::clicked, this, &TestWebEngineAccesskey::slotShowAccessKey);
}

TestWebEngineAccesskey::~TestWebEngineAccesskey()
{

}

void TestWebEngineAccesskey::slotShowAccessKey()
{
    mTestWebEngine->showAccessKeys();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestWidget *testWebEngine = new TestWidget;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

