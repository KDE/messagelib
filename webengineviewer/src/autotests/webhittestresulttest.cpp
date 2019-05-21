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

#include "webhittestresulttest.h"
#include "../webhittestresult.h"
#include <QTest>

WebHitTestResultTest::WebHitTestResultTest(QObject *parent)
    : QObject(parent)
{
}

WebHitTestResultTest::~WebHitTestResultTest()
{
}

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
    QUrl url(QStringLiteral("http://www.kde.org"));
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
    QUrl pageUrl(QStringLiteral("http://www.kde.org"));

    QVariantMap map;
    QString alternateText = QStringLiteral("FOO");
    map.insert(QStringLiteral("alternateText"), alternateText);
    bool contentEditable = true;
    map.insert(QStringLiteral("contentEditable"), contentEditable);
    bool contentSelected = true;
    map.insert(QStringLiteral("contentSelected"), contentSelected);
    QString linkTitle = QStringLiteral("GGGG");
    map.insert(QStringLiteral("linkTitle"), linkTitle);
    QUrl imageUrl(QStringLiteral("https://www.foo.net"));
    map.insert(QStringLiteral("imageUrl"), imageUrl);
    QUrl linkUrl(QStringLiteral("https://www.linux.org"));
    map.insert(QStringLiteral("linkUrl"), linkUrl);
    QUrl mediaUrl(QStringLiteral("https://www.media.org"));
    map.insert(QStringLiteral("mediaUrl"), mediaUrl);
    bool mediaPaused = true;
    map.insert(QStringLiteral("mediaPaused"), mediaPaused);
    bool mediaMuted = true;
    map.insert(QStringLiteral("mediaMuted"), mediaMuted);
    QString tagName = QStringLiteral("HHHHHH");
    map.insert(QStringLiteral("tagName"), tagName);
    QRect boundingRect(5, 7, 9, 11);
    QVariantList lstRect;
    lstRect << boundingRect.left() << boundingRect.top() << boundingRect.width() << boundingRect.height();
    map.insert(QStringLiteral("boundingRect"), lstRect);

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
    QUrl pageUrl(QStringLiteral("http://www.kde.org"));

    QVariantMap map;
    QString alternateText = QStringLiteral("FOO");
    map.insert(QStringLiteral("alternateText"), alternateText);
    bool contentEditable = true;
    map.insert(QStringLiteral("contentEditable"), contentEditable);
    bool contentSelected = true;
    map.insert(QStringLiteral("contentSelected"), contentSelected);
    QString linkTitle = QStringLiteral("GGGG");
    map.insert(QStringLiteral("linkTitle"), linkTitle);
    QUrl imageUrl(QStringLiteral("https://www.foo.net"));
    map.insert(QStringLiteral("imageUrl"), imageUrl);
    QUrl linkUrl(QStringLiteral("https://www.linux.org"));
    map.insert(QStringLiteral("linkUrl"), linkUrl);
    QUrl mediaUrl(QStringLiteral("https://www.media.org"));
    map.insert(QStringLiteral("mediaUrl"), mediaUrl);
    bool mediaPaused = true;
    map.insert(QStringLiteral("mediaPaused"), mediaPaused);
    bool mediaMuted = true;
    map.insert(QStringLiteral("mediaMuted"), mediaMuted);
    QString tagName = QStringLiteral("HHHHHH");
    map.insert(QStringLiteral("tagName"), tagName);
    QRect boundingRect(5, 7, 9, 11);
    QVariantList lstRect;
    lstRect << boundingRect.left() << boundingRect.top() << boundingRect.width() << boundingRect.height();
    map.insert(QStringLiteral("boundingRect"), lstRect);

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
