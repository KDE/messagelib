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

#include <KActionCollection>
#include <QApplication>
#include <QVBoxLayout>

#include <MessageViewer/MailWebEngineView>

TestMailDndAttachment::TestMailDndAttachment(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout;
    setLayout(vbox);
    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    vbox->addWidget(mTestWebEngine);
}

TestMailDndAttachment::~TestMailDndAttachment()
{

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
