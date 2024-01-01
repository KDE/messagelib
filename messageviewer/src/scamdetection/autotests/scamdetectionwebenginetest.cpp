/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "scamdetectionwebenginetest.h"
#include "../scamdetectionwebengine.h"
#include <QHBoxLayout>
#include <QSignalSpy>
#include <QTest>
#include <QWebEngineView>

TestWebEngineScamDetection::TestWebEngineScamDetection(QWidget *parent)
    : QWidget(parent)
    , mEngineView(new QWebEngineView(this))
    , mScamDetectionWebEngine(new MessageViewer::ScamDetectionWebEngine(this))
{
    auto hbox = new QHBoxLayout(this);
    connect(mScamDetectionWebEngine, &MessageViewer::ScamDetectionWebEngine::resultScanDetection, this, &TestWebEngineScamDetection::resultScanDetection);
    connect(mEngineView, &QWebEngineView::loadFinished, this, &TestWebEngineScamDetection::loadFinished);
    hbox->addWidget(mEngineView);
}

TestWebEngineScamDetection::~TestWebEngineScamDetection() = default;

void TestWebEngineScamDetection::setHtml(const QString &html)
{
    mEngineView->setHtml(html);
}

void TestWebEngineScamDetection::loadFinished(bool b)
{
    Q_UNUSED(b)
    mScamDetectionWebEngine->scanPage(mEngineView->page());
}

ScamDetectionWebEngineTest::ScamDetectionWebEngineTest(QObject *parent)
    : QObject(parent)
{
}

ScamDetectionWebEngineTest::~ScamDetectionWebEngineTest() = default;

void ScamDetectionWebEngineTest::scamtest_data()
{
    QTest::addColumn<QString>("html");
    QTest::addColumn<bool>("result");

    // No Scam
    QTest::newRow("noscam1") << QStringLiteral("<html><body><a href=\"www.kde.org\">kde</a></body></html>") << false;
    QTest::newRow("noscam2") << QStringLiteral("<html><body><a href=\"http://www.kde.org\" title=\"http://www.kde.org\">kde</a></body></html>") << false;
    QTest::newRow("noscam3") << QStringLiteral("<html><body><a href=\"https://www.kde.org\" title=\"https://www.kde.org\">kde</a></body></html>") << false;

    // Hexa value
    QTest::newRow("hexavalue") << QStringLiteral("<html><body><a href=\"http://125.15.55.88/\" title=\"http://0x12.0x1e.0x0A.0x00\">test</a></body></html>")
                               << true;

    // Ip
    QTest::newRow("Ip value") << QStringLiteral("<html><body><a href=\"http://127.0.0.1/\">test</a></body></html>") << false;
    QTest::newRow("Ip scam1") << QStringLiteral("<html><body><a href=\"http://125.15.55.88/\" title=\"http://www.kde.org\">test</a></body></html>") << true;
    QTest::newRow("Ip scam2") << QStringLiteral("<html><body><a href=\"http://125.15.55.88/\" title=\"http://125.15.55.88/\">test</a></body></html>") << true;

    // Href no scam
    QTest::newRow("Href no scam") << QStringLiteral("<html><body><a href=\"http://www.kde.org/\" title=\"http://www.kde.org\">test</a></body></html>") << false;

    // Redirect href
    QTest::newRow("Redirect scam") << QStringLiteral("<html><body><a href=\"http://www.google.fr/url?q=http://www.yahoo.com\">test</a></body></html>") << true;
    QTest::newRow("Redirect no scam") << QStringLiteral(
        "<html><body><a href=\"kmail:showAuditLog?log=http://www.foo.com%3http://www.bla.com\">test</a></body></html>")
                                      << false;

    // Numeric value
    QTest::newRow("numeric no scam") << QStringLiteral(
        "<html><body><a href=\"http://baseball2.2ndhalfplays.com/nested/attribs/\">http://baseball2.2ndhalfplays.com/nested/attribs</html>")
                                     << false;
    QTest::newRow("numeric scam1") << QStringLiteral("<html><body><a href=\"http://25.15.55.88/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam2") << QStringLiteral("<html><body><a href=\"http://255.0.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam3") << QStringLiteral("<html><body><a href=\"http://1.0.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam4") << QStringLiteral("<html><body><a href=\"http://255.500.1.1/\">test</a></body></html>") << true;
    QTest::newRow("numeric scam5") << QStringLiteral(
        "<html><body><a href=\"http://baseball.2ndhalfplays.com/nested/attribs/\">http://baseball2.2ndhalfplays.com/nested/attribs</html>")
                                   << true;

    QTest::newRow("scam") << QStringLiteral(
        "<html><body><a href=\"http://dfgdgsfdgsfdgsfd.foo.com/#contact@bla.org\">https://www.bli.com/manager/dedicated/index.html#/billing/mean</a></html>")
                          << true;

    QTest::newRow("scam-amp") << QStringLiteral(
        "<a "
        "href=\"https://bugs.kde.org/enter_bug.cgi?format=guided&amp;product=gcompris\">https://bugs.kde.org/"
        "enter_bug.cgi?format=guided&amp;amp;product=gcompris</a></div>")
                              << false;

    QTest::newRow("scam-encoded-url1") << QStringLiteral(
        "<a href=\"https://github.com/KDAB/KDStateMachineEditor.git|1.2\">https://github.com/KDAB/KDStateMachineEditor.git|1.2</a>")
                                       << false;

    QTest::newRow("scam-lowercase") << QStringLiteral("<a href=\"http://www.Kde.org\">http://www.Kde.org</a>") << false;
    QTest::newRow("scam-lowercase-2") << QStringLiteral("<a href=\"http://www.Kde.org/KDE/bla\">http://www.Kde.org/KDE/bla</a>") << false;
    QTest::newRow("scam-lowercase-3") << QStringLiteral(
        "<a href=\"http://code.qt.io/cgit/%7bnon-gerrit%7d/qt-labs/opencl.git\">http://code.qt.io/cgit/%7bnon-gerrit%7d/qt-labs/opencl.git</a>")
                                      << false;

    QTest::newRow("toplevelrepo") << QStringLiteral(
        "<a "
        "href=\"https://www.amazon.fr/gp/goldbox/ref=pe_btn/?nocache=1510065600354\">https://www.amazon.fr/gp/../gp/goldbox/ref=pe_btn/?nocache=1510065600354</"
        "a>") << false;

    QTest::newRow("toplevelrepo2") << QStringLiteral(
        "<a "
        "href=\"https://www.amazon.fr/gp/../gp/goldbox/ref=pe_btn/?nocache=1510065600354\">https://www.amazon.fr/gp/goldbox/ref=pe_btn/?nocache=1510065600354</"
        "a>") << false;

    QTest::newRow("toplevelrepo3") << QStringLiteral(
        "<a href=\"https://www.amazon.fr/gp/../gp/goldbox/ref=pe_d//\">https://www.amazon.fr/gp/../gp/goldbox/ref=pe_d//</a>")
                                   << false;
    QTest::newRow("endwith%22") << QStringLiteral(
        "<a href=\"http://www.kde.org/standards/kcfg/1.0/kcfg.xsd\" \"=\"\">http://www.kde.org/standards/kcfg/1.0/kcfg.xsd\"</a>")
                                << false;
    QTest::newRow("contains%5C") << QStringLiteral(
        "<a "
        "href=\"http://g-ecx.images-amazon.com/images/G/01/barcodes/blank003.jpg%5CnUse\">http://g-ecx.images-amazon.com/images/G/01/barcodes/blank003.jpg/"
        "nUse</a>") << false;
    QTest::newRow("wierd1") << QStringLiteral("<a href=\"http://www.weezevent.com?c=sys_mail\">http://www.weezevent.com?c=sys_mail</a>") << false;

    QTest::newRow("urlwithport-special443") << QStringLiteral("<a href=\"https://example.com:443/blablabla\">https://example.com:443/blablabla</a>") << false;
    QTest::newRow("urlwithport-special443-2") << QStringLiteral("<a href=\"http://example.com:443/blablabla\">http://example.com:443/blablabla</a>") << false;
    QTest::newRow("urlwithport") << QStringLiteral("<a href=\"https://example.com:465/blablabla\">https://example.com:465/blablabla</a>") << false;
    QTest::newRow("urlwithport2") << QStringLiteral("<a href=\"https://example.com:11371/blablabla\">https://example.com:11371/blablabla</a>") << false;
    QTest::newRow("urlwithport3") << QStringLiteral("<a href=\"smtps://example.com:465/blablabla\">smtps://example.com:465/blablabla</a>") << false;
    QTest::newRow("urlwithport3") << QStringLiteral("<a href=\"imaps://example.com:993/blablabla\">imaps://example.com:993/blablabla</a>") << false;
    // Bug:440635
    QTest::newRow("scam5C") << QStringLiteral(R"(<a href="https://www.google.com/search?q=%5C">https://www.google.com/search?q=%5C</a>)") << false;
    QTest::newRow("BUG440635") << QStringLiteral(
        R"(<a href="https://codereview.qt-project.org/q/topic:%22api-change-review-6.2%22+(status:open%20OR%20status:abandoned">https://codereview.qt-project.org/q/topic:%22api-change-review-6.2%22+(status:open%20OR%20status:abandoned</a>)")
                               << false;

    QTest::newRow("BUG448029") << QStringLiteral(
        R"(<a href="https://bugreports.qt.io/issues/?jql=text%20~%20%22gadget%20qml%22">https://bugreports.qt.io/issues/?jql=text%20~%20%22gadget%20qml%22</a>)")
                               << false;

    QTest::newRow("BUG448674") << QStringLiteral(
        R"(<a href="https://bugreports.qt.io/browse/QTBUG-99195" target="_blank" title="https://bugreports.qt.io/browse/qtbug-99195"> https://bugreports.qt.io/browse/QTBUG-99195</a>)")
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

#include "moc_scamdetectionwebenginetest.cpp"
