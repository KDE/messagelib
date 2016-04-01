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
    const QVariantList lst = var.toList();
    QVector<MessageViewer::WebEngineAccessKeyAnchor> anchorList;
    anchorList.reserve(lst.count());
    Q_FOREACH (const QVariant &anchor, lst) {
        anchorList << MessageViewer::WebEngineAccessKeyAnchor(anchor);
    }
    Q_EMIT accessKeySearchFinished(anchorList);
}

void TestWebEngineAccessKey::loadFinished(bool b)
{
    mEngineView->page()->runJavaScript(MessageViewer::WebEngineAccessKeyUtils::script(), invoke(this, &TestWebEngineAccessKey::handleSearchAccessKey));
}

Q_DECLARE_METATYPE(QVector<MessageViewer::WebEngineAccessKeyAnchor>)

MailWebEngineAccessKeyAnchorFromHtmlTest::MailWebEngineAccessKeyAnchorFromHtmlTest(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QVector<MessageViewer::WebEngineAccessKeyAnchor> >();
}

void MailWebEngineAccessKeyAnchorFromHtmlTest::shouldNotShowAccessKeyWhenHtmlAsNotAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, SIGNAL(accessKeySearchFinished(QVector<MessageViewer::WebEngineAccessKeyAnchor>)));
    w.setHtml(QStringLiteral("<body>foo</body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<MessageViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<MessageViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 0);
}

void MailWebEngineAccessKeyAnchorFromHtmlTest::shouldReturnOneAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, SIGNAL(accessKeySearchFinished(QVector<MessageViewer::WebEngineAccessKeyAnchor>)));
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<MessageViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<MessageViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 1);
}

void MailWebEngineAccessKeyAnchorFromHtmlTest::shouldReturnTwoAnchor()
{
    TestWebEngineAccessKey w;
    QSignalSpy accessKeySpy(&w, SIGNAL(accessKeySearchFinished(QVector<MessageViewer::WebEngineAccessKeyAnchor>)));
    w.setHtml(QStringLiteral("<body>foo<a href=\"http://www.kde.org\">foo</a><a href=\"http://www.kde.vv\">foo</a></body>"));
    QVERIFY(accessKeySpy.wait());
    QCOMPARE(accessKeySpy.count(), 1);
    const QVector<MessageViewer::WebEngineAccessKeyAnchor> resultLst = accessKeySpy.at(0).at(0).value<QVector<MessageViewer::WebEngineAccessKeyAnchor> >();
    QCOMPARE(resultLst.count(), 2);
}

QTEST_MAIN(MailWebEngineAccessKeyAnchorFromHtmlTest)
