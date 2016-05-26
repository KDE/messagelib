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

    KMime::Message *msg = new KMime::Message;

    QByteArray mail = "From: Thomas McGuire <dontspamme@gmx.net>"
                      "Subject: Fwd: Test with attachment"
                      "Date: Wed, 5 Aug 2009 10:58:27 +0200"
                      "MIME-Version: 1.0"
                      "Content-Type: Multipart/Mixed;"
                      "  boundary=\"Boundary-00=_zmUeKB+A8hGfCVZ\""
                      ""
                      ""
                      "--Boundary-00=_zmUeKB+A8hGfCVZ"
                      "Content-Type: text/plain;"
                      "  charset=\"iso-8859-15\""
                      "Content-Transfer-Encoding: 7bit"
                      "Content-Disposition: inline"
                      ""
                      "This is the first encapsulating message."
                      ""
                      "--Boundary-00=_zmUeKB+A8hGfCVZ"
                      "Content-Type: message/rfc822;"
                      "  name=\"forwarded message\""
                      "Content-Transfer-Encoding: 7bit"
                      "Content-Description: Thomas McGuire <dontspamme@gmx.net>: Test with attachment"
                      "Content-Disposition: inline"
                      ""
                      "From: Thomas McGuire <dontspamme@gmx.net>"
                      "Subject: Test with attachment"
                      "Date: Wed, 5 Aug 2009 10:57:58 +0200"
                      "MIME-Version: 1.0"
                      "Content-Type: Multipart/Mixed;"
                      "  boundary=\"Boundary-00=_WmUeKQpGb0DHyx1\""
                      ""
                      "--Boundary-00=_WmUeKQpGb0DHyx1"
                      "Content-Type: text/plain;"
                      "  charset=\"us-ascii\""
                      "Content-Transfer-Encoding: 7bit"
                      "Content-Disposition: inline"
                      ""
                      ""
                      ""
                      ""
                      "This is the second encapsulated message."
                      ""
                      "--Boundary-00=_WmUeKQpGb0DHyx1"
                      "Content-Type: text/plain;"
                      "  name=\"attachment.txt\""
                      "Content-Transfer-Encoding: 7bit"
                      "Content-Disposition: attachment;"
                      "  filename=\"attachment.txt\""
                      ""
                      "This is an attachment."
                      ""
                      "--Boundary-00=_WmUeKQpGb0DHyx1--"
                      ""
                      "--Boundary-00=_zmUeKB+A8hGfCVZ--";
    msg->setContent(mail);
    msg->parse();

    MessageViewer::Viewer *viewer = new MessageViewer::Viewer(0);
    vbox->addWidget(viewer);
    viewer->setMessage(readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox"))/*KMime::Message::Ptr(msg)*/);
    viewer->setPluginName(QStringLiteral("longheaderstyleplugin"));
}

TestMailDndAttachment::~TestMailDndAttachment()
{

}

KMime::Message::Ptr TestMailDndAttachment::readAndParseMail(const QString &mailFile)
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
    TestMailDndAttachment *testWebEngine = new TestMailDndAttachment;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
