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

#include "testjquerysupportmailwebengine.h"
#include <KActionCollection>
#include <QApplication>
#include <WebEngineViewer/WebEnginePage>
#include <QDebug>
#include <webengineview.h>
#include <QPushButton>
#include <QTextEdit>
#include <QFile>
#include <MessageViewer/Viewer>
#include <MessageViewer/AttachmentStrategy>
#include <QVBoxLayout>

TestJQuerySupportMailWebEngine::TestJQuerySupportMailWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    viewer = new MessageViewer::Viewer(nullptr, nullptr, new KActionCollection(this));
    vboxLayout->addWidget(viewer);
    viewer->setMessage(readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox")));
    viewer->setPluginName(QStringLiteral("enterprise"));
    viewer->setAttachmentStrategy(MessageViewer::AttachmentStrategy::headerOnly());

    mEditor = new QTextEdit(this);
    mEditor->setAcceptRichText(false);
    mEditor->setPlainText(QStringLiteral(
                              "qt.jQuery('img').each( function () { qt.jQuery(this).css('-webkit-transition', '-webkit-transform 2s'); qt.jQuery(this).css('-webkit-transform', 'rotate(180deg)') } ); undefined"));
    vboxLayout->addWidget(mEditor);

    QPushButton *executeQuery = new QPushButton(QStringLiteral("Execute Query"), this);
    connect(executeQuery, &QPushButton::clicked, this,
            &TestJQuerySupportMailWebEngine::slotExecuteQuery);
    vboxLayout->addWidget(executeQuery);
}

TestJQuerySupportMailWebEngine::~TestJQuerySupportMailWebEngine()
{
}

KMime::Message::Ptr TestJQuerySupportMailWebEngine::readAndParseMail(const QString &mailFile)
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

void TestJQuerySupportMailWebEngine::slotExecuteQuery()
{
    const QString code = mEditor->toPlainText();
    if (!code.isEmpty()) {
        viewer->runJavaScript(code);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestJQuerySupportMailWebEngine *testWebEngine = new TestJQuerySupportMailWebEngine;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
