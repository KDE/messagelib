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


#include "mailwebengineaccesskeyanchorfromhtmltest.h"
#include "../mailwebengineaccesskeyutils.h"
#include <QTest>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QSignalSpy>

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

TestWebEngineAccessKey::TestWebEngineAccessKey(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);
    mEngineView = new QWebEngineView(this);
    connect(mEngineView, &QWebEngineView::loadFinished, this, &TestWebEngineAccessKey::loadFinished);
    hbox->addWidget(mEngineView);
}

TestWebEngineAccessKey::~TestWebEngineAccessKey()
{

}

void TestWebEngineAccessKey::setHtml(const QString &html)
{
    mEngineView->setHtml(html);
}

void TestWebEngineAccessKey::handleSearchAccessKey(const QVariant &var)
{
    Q_EMIT accessKeySearchFinished(var);
}

void TestWebEngineAccessKey::loadFinished(bool b)
{
    mEngineView->page()->runJavaScript(MessageViewer::MailWebEngineAccessKeyUtils::script(), invoke(this, &TestWebEngineAccessKey::handleSearchAccessKey));
}

MailWebEngineAccessKeyAnchorFromHtmlTest::MailWebEngineAccessKeyAnchorFromHtmlTest(QObject *parent)
    : QObject(parent)
{

}

void MailWebEngineAccessKeyAnchorFromHtmlTest::shouldNotShowAccessKeyWhenHtmlAsNotAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, SIGNAL(accessKeySearchFinished(QVariant)));
    w.setHtml(QStringLiteral("<body>foo</body>"));
    QVERIFY(accessKeySpy.wait());

}




QTEST_MAIN(MailWebEngineAccessKeyAnchorFromHtmlTest)
