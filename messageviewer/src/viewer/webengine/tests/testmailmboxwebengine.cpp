/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "testmailmboxwebengine.h"

#include <KActionCollection>
#include <KMime/Content>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QVBoxLayout>

#include <MessageViewer/MailWebEngineView>
#include <MessageViewer/Viewer>

TestMailMBoxWebEngine::TestMailMBoxWebEngine(QWidget *parent)
    : QWidget(parent)
{
    auto vbox = new QVBoxLayout(this);
    auto viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(this));
    vbox->addWidget(viewer);
    viewer->setMessage(readAndParseMail(QStringLiteral("html.mbox")));
    viewer->setPluginName(QStringLiteral("longheaderstyleplugin"));
}

TestMailMBoxWebEngine::~TestMailMBoxWebEngine() = default;

KMime::Message::Ptr TestMailMBoxWebEngine::readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1StringView(MAIL_DATA_DIR) + QLatin1Char('/') + mailFile);
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    qDebug() << ba;
    const QByteArray data = ba;
    Q_ASSERT(!data.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    auto testWebEngine = new TestMailMBoxWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}

#include "moc_testmailmboxwebengine.cpp"
