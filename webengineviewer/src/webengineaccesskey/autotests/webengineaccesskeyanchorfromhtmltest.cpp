/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "webengineaccesskeyanchorfromhtmltest.h"
#include "../webengineaccesskeyutils.h"
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QWebEngineView>
#include <WebEngineViewer/WebEngineManageScript>

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
    auto hbox = new QHBoxLayout(this);
    mEngineView = new QWebEngineView(this);
    connect(mEngineView, &QWebEngineView::loadFinished, this, &TestWebEngineAccessKey::loadFinished);
    hbox->addWidget(mEngineView);
}

TestWebEngineAccessKey::~TestWebEngineAccessKey() = default;

void TestWebEngineAccessKey::setHtml(const QString &html)
{
    mEngineView->setHtml(html);
}

void TestWebEngineAccessKey::handleSearchAccessKey(const QVariant &var)
{
    const QVariantList lst = var.toList();
    QList<WebEngineViewer::WebEngineAccessKeyAnchor> anchorList;
    anchorList.reserve(lst.count());
    for (const QVariant &anchor : lst) {
        anchorList << WebEngineViewer::WebEngineAccessKeyAnchor(anchor);
    }
    Q_EMIT accessKeySearchFinished(anchorList);
}

void TestWebEngineAccessKey::loadFinished(bool b)
{
    Q_UNUSED(b)
    mEngineView->page()->runJavaScript(WebEngineViewer::WebEngineAccessKeyUtils::accessKeyScript(),
                                       WebEngineViewer::WebEngineManageScript::scriptWordId(),
                                       invoke(this, &TestWebEngineAccessKey::handleSearchAccessKey));
}

Q_DECLARE_METATYPE(QList<WebEngineViewer::WebEngineAccessKeyAnchor>)

WebEngineAccessKeyAnchorFromHtmlTest::WebEngineAccessKeyAnchorFromHtmlTest(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QList<WebEngineViewer::WebEngineAccessKeyAnchor>>();
    QStandardPaths::setTestModeEnabled(true);
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldNotShowAccessKeyWhenHtmlAsNotAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo</body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const auto resultLst = accessKeySpy.at(0).at(0).value<QList<WebEngineViewer::WebEngineAccessKeyAnchor>>();
    QCOMPARE(resultLst.count(), 0);
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldReturnOneAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const auto resultLst = accessKeySpy.at(0).at(0).value<QList<WebEngineViewer::WebEngineAccessKeyAnchor>>();
    QCOMPARE(resultLst.count(), 1);
}

void WebEngineAccessKeyAnchorFromHtmlTest::shouldReturnTwoAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, &TestWebEngineAccessKey::accessKeySearchFinished);
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a><a href=\"http://www.kde.vv\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const auto resultLst = accessKeySpy.at(0).at(0).value<QList<WebEngineViewer::WebEngineAccessKeyAnchor>>();
    QCOMPARE(resultLst.count(), 2);
}

QTEST_MAIN(WebEngineAccessKeyAnchorFromHtmlTest)

#include "moc_webengineaccesskeyanchorfromhtmltest.cpp"
