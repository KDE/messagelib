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


#include "testmaildndattachment.h"

#include <KMime/Content>
#include <KActionCollection>
#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QDebug>

#include <MessageViewer/MailWebEngineView>
#include <MessageViewer/Viewer>

TestMailDndAttachment::TestMailDndAttachment(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    setLayout(vbox);

    MessageViewer::Viewer *viewer = new MessageViewer::Viewer(0);
    vbox->addWidget(viewer);
    //viewer->setPluginName(parser.value(QStringLiteral("headerstyleplugin")));
    viewer->setMessage(readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox")));
}

TestMailDndAttachment::~TestMailDndAttachment()
{

}

KMime::Message::Ptr TestMailDndAttachment::readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1String(MAIL_DATA_DIR) + QLatin1Char('/') + mailFile);
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    qDebug()<<" file"<<file.fileName();
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    qDebug()<<" data "<< data;
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
    TestMailDndAttachment *testWebEngine = new TestMailDndAttachment;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
