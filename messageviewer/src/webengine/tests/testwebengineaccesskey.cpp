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
#include "viewer/webengine/mailwebengineaccesskey.h"
#include "viewer/webengine/mailwebengineview.h"
#include "messageviewer/messageviewersettings.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QWebEngineSettings>
#include <QStandardPaths>

#include <KActionCollection>


TestWebEngineAccesskey::TestWebEngineAccesskey(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    MessageViewer::MessageViewerSettings::self()->setAccessKeyEnabled(true);
    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestWebEngineAccesskey::~TestWebEngineAccesskey()
{

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestWebEngineAccesskey *testWebEngine = new TestWebEngineAccesskey;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
