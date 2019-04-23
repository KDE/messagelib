/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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
#include "webengineaccesskeyanchorfromhtmltest.h"
#include "../webengineaccesskeyutils.h"
#include <WebEngineViewer/WebEngineManageScript>
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
    const QVariantList lst = var.toList();
    QVector<WebEngineViewer::WebEngineAccessKeyAnchor> anchorList;
    anchorList.reserve(lst.count());
    for (const QVariant &anchor : lst) {
        anchorList << WebEngineViewer::WebEngineAccessKeyAnchor(anchor);
    }
    Q_EMIT accessKeySearchFinished(anchorList);
}

void TestWebEngineAccessKey::loadFinished(bool b)
{
    Q_UNUSED(b);
    mEngineView->page()->runJavaScript(WebEngineViewer::WebEngineAccessKeyUtils::script(),
                                       WebEngineViewer::WebEngineManageScript::scriptWordId(),
                                       invoke(this, &TestWebEngineAccessKey::handleSearchAccessKey));
}

Q_DECLARE_METATYPE(QVector<WebEngineViewer::WebEngineAccessKeyAnchor>)

WebEngineAccessKeyAnchorFromHtmlTest::WebEngineAccessKeyAnchorFromHtmlTest(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QVector<WebEngineViewer::WebEngineAccessKeyAnchor> >();
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldNotShowAccessKeyWhenHtmlAsNotAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo</body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<WebEngineViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<WebEngineViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 0);
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldReturnOneAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<WebEngineViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<WebEngineViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 1);
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldReturnTwoAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a><a href=\"http://www.kde.vv\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<WebEngineViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<WebEngineViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 2);
}

QTEST_MAIN(WebEngineAccessKeyAnchorFromHtmlTest)
