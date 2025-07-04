/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testwebengineaccesskey.h"
using namespace Qt::Literals::StringLiterals;

#include "../mailwebengineview.h"

#include "messageviewer/messageviewersettings.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QWebEngineSettings>

#include <QLabel>
#include <QPushButton>

TestWidget::TestWidget(QWidget *parent)
    : QWidget(parent)
{
    WebEngineViewer::MessageViewerSettings::self()->setAccessKeyEnabled(true);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->setContentsMargins(0, 0, 0, 0);
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
    QLabel *label = new QLabel(u"WebEngine"_s);
    vboxLayout->addWidget(label);

    mTestWebEngine = new WebEngineViewer::MailWebEngineView(new KActionCollection(this), this);
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(u"http://www.kde.org"_s));
    QPushButton *searchAccessKey = new QPushButton(u"AccessKey"_s, this);
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

#include "moc_testwebengineaccesskey.cpp"
