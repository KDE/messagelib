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

#include "testwebenginescrolladdattachment.h"
#include "webengine/webenginescript.h"

#include <KActionCollection>
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebEngineSettings>

#include <viewer/webengine/mailwebengineview.h>

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFunction)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFunction)(result);
    }
};

template<typename Arg, typename R, typename C>

InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFunction)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFunction};
    return wrapper;
}

TestWebEngineScrollAddAttachment::TestWebEngineScrollAddAttachment(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);

    mTestWebEngine = new MessageViewer::MailWebEngineView(new KActionCollection(this), this);
    mTestWebEngine->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    vboxLayout->addWidget(mTestWebEngine);
    mTestWebEngine->load(QUrl(QStringLiteral("http://www.kde.org")));
    QPushButton *scrollToButton = new QPushButton(QStringLiteral("Scroll to Attachment"), this);
    vboxLayout->addWidget(scrollToButton);
    connect(scrollToButton, &QPushButton::clicked, this, &TestWebEngineScrollAddAttachment::slotScrollToAttachment);

}

void TestWebEngineScrollAddAttachment::handleScrollToAnchor(const QVariant &result)
{
    qDebug() << " result "<<result;
    if (result.isValid()) {
        const QList<QVariant> lst = result.toList();
        if (lst.count() == 2) {
            const QPoint pos(lst.at(0).toInt(), lst.at(1).toInt());
            mTestWebEngine->page()->runJavaScript(MessageViewer::WebEngineScript::scrollToPosition(pos));
        }
    }
}

void TestWebEngineScrollAddAttachment::slotScrollToAttachment()
{
    mTestWebEngine->page()->runJavaScript(MessageViewer::WebEngineScript::searchElementPosition(QStringLiteral("#footer_text")), invoke(this, &TestWebEngineScrollAddAttachment::handleScrollToAnchor));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    TestWebEngineScrollAddAttachment *testWebEngine = new TestWebEngineScrollAddAttachment;
    testWebEngine->show();
    const int ret = app.exec();
    return ret;
}
