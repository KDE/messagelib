/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "testmailmboxwebengine.h"

#include <KMime/Content>
#include <KActionCollection>
#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>

#include <MessageViewer/MailWebEngineView>
#include <MessageViewer/Viewer>

TestMailMBoxWebEngine::TestMailMBoxWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    MessageViewer::Viewer *viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(
                                                                  this));
    vbox->addWidget(viewer);
    viewer->setMessage(readAndParseMail(QStringLiteral("html.mbox")));
    viewer->setPluginName(QStringLiteral("longheaderstyleplugin"));
}

TestMailMBoxWebEngine::~TestMailMBoxWebEngine()
{
}

KMime::Message::Ptr TestMailMBoxWebEngine::readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1String(MAIL_DATA_DIR) + QLatin1Char('/') + mailFile);
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
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestMailMBoxWebEngine *testWebEngine = new TestMailMBoxWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
