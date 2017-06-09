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

#include "scamdetectionwebenginetest.h"
#include "../scamdetectionwebengine.h"
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QTest>
#include <QWebEngineView>

TestWebEngineScamDetection::TestWebEngineScamDetection(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hbox = new QHBoxLayout(this);
    mEngineView = new QWebEngineView(this);
    mScamDetectionWebEngine = new MessageViewer::ScamDetectionWebEngine(this);
    connect(mScamDetectionWebEngine, &MessageViewer::ScamDetectionWebEngine::resultScanDetection,
            this, &TestWebEngineScamDetection::resultScanDetection);
    connect(mEngineView, &QWebEngineView::loadFinished, this,
            &TestWebEngineScamDetection::loadFinished);
    hbox->addWidget(mEngineView);
}

TestWebEngineScamDetection::~TestWebEngineScamDetection()
{
}

void TestWebEngineScamDetection::setHtml(const QString &html)
{
    mEngineView->setHtml(html);
}

void TestWebEngineScamDetection::loadFinished(bool b)
{
    Q_UNUSED(b);
    mScamDetectionWebEngine->scanPage(mEngineView->page());
}

ScamDetectionWebEngineTest::ScamDetectionWebEngineTest(QObject *parent)
    : QObject(parent)
{
}

ScamDetectionWebEngineTest::~ScamDetectionWebEngineTest()
{
}

void ScamDetectionWebEngineTest::scamtest_data()
{
    QTest::addColumn<QString>("html");
    QTest::addColumn<bool>("result");

    //No Scam
    QTest::newRow("noscam1") << QStringLiteral(
        "<html><body><a href=\"www.kde.org\">kde</a></body></html>") << false;
    QTest::newRow("noscam2") << QStringLiteral(
        "<html><body><a href=\"http://www.kde.org\" title=\"http://www.kde.org\">kde</a></body></html>")
                             << false;
    QTest::newRow("noscam3") << QStringLiteral(
        "<html><body><a href=\"https://www.kde.org\" title=\"https://www.kde.org\">kde</a></body></html>")
                             << false;

    //Hexa value
    QTest::newRow("hexavalue") << QStringLiteral(
        "<html><body><a href=\"http://125.15.55.88/\" title=\"http://0x12.0x1e.0x0A.0x00\">test</a></body></html>")
                               << true;

    //Ip
    QTest::newRow("Ip value") << QStringLiteral(
        "<html><body><a href=\"http://127.0.0.1/\">test</a></body></html>") << false;
    QTest::newRow("Ip scam1") << QStringLiteral(
        "<html><body><a href=\"http://125.15.55.88/\" title=\"http://www.kde.org\">test</a></body></html>")
                              << true;
    QTest::newRow("Ip scam2") << QStringLiteral(
        "<html><body><a href=\"http://125.15.55.88/\" title=\"http://125.15.55.88/\">test</a></body></html>")
                              << true;

    //Href no scam
    QTest::newRow("Href no scam") << QStringLiteral(
        "<html><body><a href=\"http://www.kde.org/\" title=\"http://www.kde.org\">test</a></body></html>")
                                  << false;

    //Redirect href
    QTest::newRow("Redirect scam") << QStringLiteral(
        "<html><body><a href=\"http://www.google.fr/url?q=http://www.yahoo.com\">test</a></body></html>")
                                   << true;
    QTest::newRow("Redirect no scam") << QStringLiteral(
        "<html><body><a href=\"kmail:showAuditLog?log=http://www.foo.com%3http://www.bla.com\">test</a></body></html>")
                                      << false;

    //Numeric value
    QTest::newRow("numeric no scam") << QStringLiteral(
        "<html><body><a href=\"http://baseball2.2ndhalfplays.com/nested/attribs/\">http://baseball2.2ndhalfplays.com/nested/attribs</html>")
                                     << false;
    QTest::newRow("numeric scam1") << QStringLiteral(
        "<html><body><a href=\"http://25.15.55.88/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam2") << QStringLiteral(
        "<html><body><a href=\"http://255.0.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam3") << QStringLiteral(
        "<html><body><a href=\"http://1.0.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam4") << QStringLiteral(
        "<html><body><a href=\"http://255.500.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric no scam5") << QStringLiteral(
        "<html><body><a href=\"http://baseball.2ndhalfplays.com/nested/attribs/\">http://baseball2.2ndhalfplays.com/nested/attribs</html>")
                                      << false;
}

void ScamDetectionWebEngineTest::scamtest()
{
    QFETCH(QString, html);
    QFETCH(bool, result);

    TestWebEngineScamDetection scamDetection;
    QSignalSpy scamDetectionSpy(&scamDetection, SIGNAL(resultScanDetection(bool)));
    scamDetection.setHtml(html);
    QVERIFY(scamDetectionSpy.wait());
    QCOMPARE(scamDetectionSpy.count(), 1);
    const bool scamResult = scamDetectionSpy.at(0).at(0).toBool();
    QCOMPARE(scamResult, result);
}

QTEST_MAIN(ScamDetectionWebEngineTest)
