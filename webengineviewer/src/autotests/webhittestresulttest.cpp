/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "webhittestresulttest.h"
using namespace Qt::Literals::StringLiterals;

#include "../webhittestresult.h"
#include <QTest>

WebHitTestResultTest::WebHitTestResultTest(QObject *parent)
    : QObject(parent)
{
}

WebHitTestResultTest::~WebHitTestResultTest() = default;

void WebHitTestResultTest::shouldHaveDefaultValues()
{
    WebEngineViewer::WebHitTestResult result;
    QVERIFY(result.alternateText().isEmpty());
    QVERIFY(result.boundingRect().isNull());
    QVERIFY(!result.imageUrl().isValid());
    QVERIFY(!result.isContentEditable());
    QVERIFY(!result.isContentSelected());
    QVERIFY(result.isNull());
    QVERIFY(result.linkTitle().isEmpty());
    QVERIFY(!result.linkUrl().isValid());
    QVERIFY(!result.mediaUrl().isValid());
    QVERIFY(!result.mediaPaused());
    QVERIFY(!result.mediaMuted());
    QVERIFY(result.pos().isNull());
    QVERIFY(result.tagName().isEmpty());
    QVERIFY(!result.pageUrl().isValid());
}

void WebHitTestResultTest::shouldAssignPosAndUrl()
{
    QPoint pos(5, 5);
    QUrl url(u"http://www.kde.org"_s);
    WebEngineViewer::WebHitTestResult result(pos, url, QVariant());
    QVERIFY(result.alternateText().isEmpty());
    QVERIFY(result.boundingRect().isNull());
    QVERIFY(!result.imageUrl().isValid());
    QVERIFY(!result.isContentEditable());
    QVERIFY(!result.isContentSelected());
    QVERIFY(result.isNull());
    QVERIFY(result.linkTitle().isEmpty());
    QVERIFY(!result.mediaUrl().isValid());
    QVERIFY(!result.mediaPaused());
    QVERIFY(!result.mediaMuted());
    QVERIFY(result.tagName().isEmpty());
    QVERIFY(!result.linkUrl().isValid());

    QCOMPARE(result.pageUrl(), url);
    QCOMPARE(result.pos(), pos);
}

void WebHitTestResultTest::shouldAssignFromQVariant()
{
    QPoint pos(5, 5);
    QUrl pageUrl(u"http://www.kde.org"_s);

    QVariantMap map;
    QString alternateText = u"FOO"_s;
    map.insert(u"alternateText"_s, alternateText);
    bool contentEditable = true;
    map.insert(u"contentEditable"_s, contentEditable);
    bool contentSelected = true;
    map.insert(u"contentSelected"_s, contentSelected);
    QString linkTitle = u"GGGG"_s;
    map.insert(u"linkTitle"_s, linkTitle);
    QUrl imageUrl(u"https://www.foo.net"_s);
    map.insert(u"imageUrl"_s, imageUrl);
    QUrl linkUrl(u"https://www.linux.org"_s);
    map.insert(u"linkUrl"_s, linkUrl);
    QUrl mediaUrl(u"https://www.media.org"_s);
    map.insert(u"mediaUrl"_s, mediaUrl);
    bool mediaPaused = true;
    map.insert(u"mediaPaused"_s, mediaPaused);
    bool mediaMuted = true;
    map.insert(u"mediaMuted"_s, mediaMuted);
    QString tagName = u"HHHHHH"_s;
    map.insert(u"tagName"_s, tagName);
    QRect boundingRect(5, 7, 9, 11);
    QVariantList lstRect;
    lstRect << boundingRect.left() << boundingRect.top() << boundingRect.width() << boundingRect.height();
    map.insert(u"boundingRect"_s, lstRect);

    WebEngineViewer::WebHitTestResult result(pos, pageUrl, map);

    QCOMPARE(result.alternateText(), alternateText);
    QCOMPARE(result.boundingRect(), boundingRect);
    QCOMPARE(result.imageUrl(), imageUrl);
    QCOMPARE(result.isContentEditable(), contentEditable);
    QCOMPARE(result.isContentSelected(), contentSelected);
    QCOMPARE(result.isNull(), false);
    QCOMPARE(result.linkTitle(), linkTitle);
    QCOMPARE(result.linkUrl(), linkUrl);
    QCOMPARE(result.mediaUrl(), mediaUrl);
    QCOMPARE(result.mediaPaused(), mediaPaused);
    QCOMPARE(result.mediaMuted(), mediaMuted);
    QCOMPARE(result.pos(), pos);
    QCOMPARE(result.tagName(), tagName);
    QCOMPARE(result.pageUrl(), pageUrl);
}

void WebHitTestResultTest::shouldCopyWebHitTestResult()
{
    QPoint pos(5, 5);
    QUrl pageUrl(u"http://www.kde.org"_s);

    QVariantMap map;
    QString alternateText = u"FOO"_s;
    map.insert(u"alternateText"_s, alternateText);
    bool contentEditable = true;
    map.insert(u"contentEditable"_s, contentEditable);
    bool contentSelected = true;
    map.insert(u"contentSelected"_s, contentSelected);
    QString linkTitle = u"GGGG"_s;
    map.insert(u"linkTitle"_s, linkTitle);
    QUrl imageUrl(u"https://www.foo.net"_s);
    map.insert(u"imageUrl"_s, imageUrl);
    QUrl linkUrl(u"https://www.linux.org"_s);
    map.insert(u"linkUrl"_s, linkUrl);
    QUrl mediaUrl(u"https://www.media.org"_s);
    map.insert(u"mediaUrl"_s, mediaUrl);
    bool mediaPaused = true;
    map.insert(u"mediaPaused"_s, mediaPaused);
    bool mediaMuted = true;
    map.insert(u"mediaMuted"_s, mediaMuted);
    QString tagName = u"HHHHHH"_s;
    map.insert(u"tagName"_s, tagName);
    QRect boundingRect(5, 7, 9, 11);
    QVariantList lstRect;
    lstRect << boundingRect.left() << boundingRect.top() << boundingRect.width() << boundingRect.height();
    map.insert(u"boundingRect"_s, lstRect);

    const WebEngineViewer::WebHitTestResult result1(pos, pageUrl, map);
    WebEngineViewer::WebHitTestResult result = result1;

    QCOMPARE(result.alternateText(), alternateText);
    QCOMPARE(result.boundingRect(), boundingRect);
    QCOMPARE(result.imageUrl(), imageUrl);
    QCOMPARE(result.isContentEditable(), contentEditable);
    QCOMPARE(result.isContentSelected(), contentSelected);
    QCOMPARE(result.isNull(), false);
    QCOMPARE(result.linkTitle(), linkTitle);
    QCOMPARE(result.linkUrl(), linkUrl);
    QCOMPARE(result.mediaUrl(), mediaUrl);
    QCOMPARE(result.mediaPaused(), mediaPaused);
    QCOMPARE(result.mediaMuted(), mediaMuted);
    QCOMPARE(result.pos(), pos);
    QCOMPARE(result.tagName(), tagName);
    QCOMPARE(result.pageUrl(), pageUrl);
}

QTEST_MAIN(WebHitTestResultTest)

#include "moc_webhittestresulttest.cpp"
