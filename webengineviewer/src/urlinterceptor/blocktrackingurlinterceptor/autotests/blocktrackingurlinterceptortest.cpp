/*
   SPDX-FileCopyrightText: 2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blocktrackingurlinterceptortest.h"

#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <WebEngineViewer/BlockTrackingUrlInterceptor>

using namespace Qt::Literals::StringLiterals;
QTEST_GUILESS_MAIN(BlockTrackingUrlInterceptorTest)

BlockTrackingUrlInterceptorTest::BlockTrackingUrlInterceptorTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

void BlockTrackingUrlInterceptorTest::shouldHaveDefaultValue()
{
    WebEngineViewer::BlockTrackingUrlInterceptor interceptor;
    QVERIFY(!interceptor.enabledMailTrackingInterceptor());
}

void BlockTrackingUrlInterceptorTest::shouldNotInterceptWhenDisabled()
{
    WebEngineViewer::BlockTrackingUrlInterceptor interceptor;
    QSignalSpy spy(&interceptor, &WebEngineViewer::BlockTrackingUrlInterceptor::trackingFound);
    QVERIFY(!interceptor.interceptRequest(QUrl(u"https://t.yesware.com/t/abcd/0.png"_s)));
    QCOMPARE(spy.count(), 0);
}

void BlockTrackingUrlInterceptorTest::shouldInterceptTracker_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("companyName");

    QTest::newRow("yesware") << QUrl(u"https://t.yesware.com/t/abcd/0.png"_s) << u"Yesware"_s;
    QTest::newRow("yesware-app") << QUrl(u"https://app.yesware.com/t/abcd/0.png"_s) << u"Yesware"_s;
    QTest::newRow("hubspot-signaux") << QUrl(u"https://t.signauxtrois.com/e1t/o/abcd"_s) << u"HubSpot"_s;
    QTest::newRow("hubspot-hsms") << QUrl(u"https://t.hsms06.com/e2t/o/abcd"_s) << u"HubSpot"_s;
    QTest::newRow("hubspot-sidekick") << QUrl(u"https://t.sidekickopen07.com/abcd"_s) << u"HubSpot"_s;
    QTest::newRow("hubspot-getsidekick") << QUrl(u"https://track.getsidekick.com/abcd"_s) << u"HubSpot"_s;
    QTest::newRow("sendgrid") << QUrl(u"https://u123.ct.sendgrid.net/wf/open?upn=abcd"_s) << u"SendGrid"_s;
    QTest::newRow("mailchimp") << QUrl(u"https://xyz.list-manage.com/track/open.php?u=123&id=456"_s) << u"MailChimp"_s;
    QTest::newRow("streak") << QUrl(u"https://mailfoogae.appspot.com/t?sender=abcd"_s) << u"Streak"_s;
    QTest::newRow("superhuman") << QUrl(u"https://r.superhuman.com/abcd.gif"_s) << u"Superhuman"_s;
    QTest::newRow("postmark") << QUrl(u"https://pstmrk.it/open/abcd"_s) << u"Postmark"_s;
    QTest::newRow("mailtrack") << QUrl(u"https://mltrk.io/pixel/abcd"_s) << u"Mailtrack.io"_s;
    QTest::newRow("mixmax") << QUrl(u"https://email.mixmax.com/abcd"_s) << u"MixMax"_s;
    QTest::newRow("convertkit") << QUrl(u"https://foo.convertkit-mail2.com/o/abcd"_s) << u"ConvertKit"_s;
    QTest::newRow("sendinblue") << QUrl(u"https://sendibt2.com/track/openc.php?x=1"_s) << u"Sendinblue"_s;
    QTest::newRow("intercom") << QUrl(u"https://via.intercom.io/o/abcd"_s) << u"Intercom"_s;
    QTest::newRow("outreach") << QUrl(u"https://app.outreach.io/api/abcd"_s) << u"Outreach"_s;
    QTest::newRow("bananatag") << QUrl(u"https://bl-1.com/abcd.gif"_s) << u"Banana Tag"_s;
    QTest::newRow("boomerang") << QUrl(u"https://mailstat.us/tr/abcd"_s) << u"Boomerang"_s;
    QTest::newRow("icontact") << QUrl(u"https://click.icptrack.com/icp/track.php"_s) << u"iContact"_s;
    QTest::newRow("campaignmonitor") << QUrl(u"https://cmail1.com/t/r-abcd"_s) << u"Campaign Monitor"_s;
    QTest::newRow("litmus") << QUrl(u"https://emltrk.com/abcd?d=1"_s) << u"Litmus"_s;
    QTest::newRow("cirrusinsight") << QUrl(u"https://tracking.cirrusinsight.com/abcd"_s) << u"Cirrus Insight"_s;
    QTest::newRow("mandrill") << QUrl(u"https://mandrillapp.com/track/open.php?u=1"_s) << u"Mandrill"_s;
    QTest::newRow("toutapp") << QUrl(u"https://go.toutapp.com/abcd"_s) << u"ToutApp"_s;
    QTest::newRow("frontapp") << QUrl(u"https://web.frontapp.com/api/abcd"_s) << u"Front App"_s;
    QTest::newRow("mailgun") << QUrl(u"https://email.mg.example.com/e/o/abcdefghijklmnop"_s) << u"Mailgun"_s;
    QTest::newRow("segment") << QUrl(u"https://email.segment.com/e/o/abcdefghijklmnop"_s) << u"Segment"_s;
    QTest::newRow("vocus") << QUrl(u"https://tracking.vocus.io/abcd"_s) << u"Vocus"_s;
    // Host names are case insensitive, a tracker must not escape detection by changing the case.
    QTest::newRow("uppercase-host") << QUrl(u"https://T.YESWARE.COM/t/abcd/0.png"_s) << u"Yesware"_s;
}

void BlockTrackingUrlInterceptorTest::shouldInterceptTracker()
{
    QFETCH(QUrl, url);
    QFETCH(QString, companyName);

    WebEngineViewer::BlockTrackingUrlInterceptor interceptor;
    interceptor.setEnabledMailTrackingInterceptor(true);
    QSignalSpy spy(&interceptor, &WebEngineViewer::BlockTrackingUrlInterceptor::trackingFound);
    QVERIFY(interceptor.interceptRequest(url));
    QCOMPARE(spy.count(), 1);
    const auto tracker = spy.at(0).at(0).value<WebEngineViewer::BlockTrackingUrlInterceptor::TrackerBlackList>();
    QCOMPARE(tracker.mCompanyName, companyName);
    QVERIFY(!tracker.mCompanyUrl.isEmpty());
}

void BlockTrackingUrlInterceptorTest::shouldNotInterceptRegularUrl_data()
{
    QTest::addColumn<QUrl>("url");

    QTest::newRow("kde") << QUrl(u"https://www.kde.org/index.html"_s);
    QTest::newRow("wikimedia") << QUrl(u"https://upload.wikimedia.org/wikipedia/commons/a/ab/foo.png"_s);
    QTest::newRow("cdn") << QUrl(u"https://cdn.example.com/images/logo.png"_s);
    QTest::newRow("gmail") << QUrl(u"https://mail.google.com/mail/u/0/#inbox"_s);
    QTest::newRow("data") << QUrl(u"data:image/png;base64,iVBORw0KGgo="_s);
    QTest::newRow("empty") << QUrl();
}

void BlockTrackingUrlInterceptorTest::shouldNotInterceptRegularUrl()
{
    QFETCH(QUrl, url);

    WebEngineViewer::BlockTrackingUrlInterceptor interceptor;
    interceptor.setEnabledMailTrackingInterceptor(true);
    QSignalSpy spy(&interceptor, &WebEngineViewer::BlockTrackingUrlInterceptor::trackingFound);
    QVERIFY(!interceptor.interceptRequest(url));
    QCOMPARE(spy.count(), 0);
}

#include "moc_blocktrackingurlinterceptortest.cpp"
