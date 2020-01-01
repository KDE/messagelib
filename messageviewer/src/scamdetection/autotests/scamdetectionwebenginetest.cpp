/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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
    QTest::newRow("numeric scam5") << QStringLiteral(
        "<html><body><a href=\"http://baseball.2ndhalfplays.com/nested/attribs/\">http://baseball2.2ndhalfplays.com/nested/attribs</html>")
                                   << true;

    QTest::newRow("scam") << QStringLiteral(
        "<html><body><a href=\"http://dfgdgsfdgsfdgsfd.foo.com/#contact@bla.org\">https://www.bli.com/manager/dedicated/index.html#/billing/mean</a></html>")
                          << true;

    QTest::newRow("scam-amp") << QStringLiteral(
        "<a href=\"https://bugs.kde.org/enter_bug.cgi?format=guided&amp;product=gcompris\">https://bugs.kde.org/enter_bug.cgi?format=guided&amp;amp;product=gcompris</a></div>")
                              << false;

    QTest::newRow("scam-encoded-url1") << QStringLiteral(
        "<a href=\"https://github.com/KDAB/KDStateMachineEditor.git|1.2\">https://github.com/KDAB/KDStateMachineEditor.git|1.2</a>")
                                       << false;

    QTest::newRow("scam-lowercase") << QStringLiteral(
        "<a href=\"http://www.Kde.org\">http://www.Kde.org</a>")
                                    << false;
    QTest::newRow("scam-lowercase-2") << QStringLiteral(
        "<a href=\"http://www.Kde.org/KDE/bla\">http://www.Kde.org/KDE/bla</a>")
                                      << false;
    QTest::newRow("scam-lowercase-3") << QStringLiteral(
        "<a href=\"http://code.qt.io/cgit/%7bnon-gerrit%7d/qt-labs/opencl.git\">http://code.qt.io/cgit/%7bnon-gerrit%7d/qt-labs/opencl.git</a>")
                                      << false;

    QTest::newRow("toplevelrepo") << QStringLiteral(
        "<a href=\"https://www.amazon.fr/gp/goldbox/ref=pe_btn/?nocache=1510065600354\">https://www.amazon.fr/gp/../gp/goldbox/ref=pe_btn/?nocache=1510065600354</a>")
                                  << false;

    QTest::newRow("toplevelrepo2") << QStringLiteral(
        "<a href=\"https://www.amazon.fr/gp/../gp/goldbox/ref=pe_btn/?nocache=1510065600354\">https://www.amazon.fr/gp/goldbox/ref=pe_btn/?nocache=1510065600354</a>")
                                   << false;

    QTest::newRow("toplevelrepo3") << QStringLiteral(
        "<a href=\"https://www.amazon.fr/gp/../gp/goldbox/ref=pe_d//\">https://www.amazon.fr/gp/../gp/goldbox/ref=pe_d//</a>")
                                   << false;
    QTest::newRow("endwith%22") << QStringLiteral(
        "<a href=\"http://www.kde.org/standards/kcfg/1.0/kcfg.xsd\" \"=\"\">http://www.kde.org/standards/kcfg/1.0/kcfg.xsd\"</a>")
                                << false;

    QTest::newRow("contains%5C") << QStringLiteral(
        "<a href=\"http://g-ecx.images-amazon.com/images/G/01/barcodes/blank003.jpg%5CnUse\">http://g-ecx.images-amazon.com/images/G/01/barcodes/blank003.jpg/nUse</a>")
                                 << false;
    QTest::newRow("wierd1") << QStringLiteral(
        "<a href=\"http://www.weezevent.com?c=sys_mail\">http://www.weezevent.com?c=sys_mail</a>")
                            << false;
}

void ScamDetectionWebEngineTest::scamtest()
{
    QFETCH(QString, html);
    QFETCH(bool, result);

    TestWebEngineScamDetection scamDetection;
    QSignalSpy scamDetectionSpy(&scamDetection, &TestWebEngineScamDetection::resultScanDetection);
    scamDetection.setHtml(html);
    QVERIFY(scamDetectionSpy.wait());
    QCOMPARE(scamDetectionSpy.count(), 1);
    const bool scamResult = scamDetectionSpy.at(0).at(0).toBool();
    QCOMPARE(scamResult, result);
}

QTEST_MAIN(ScamDetectionWebEngineTest)
