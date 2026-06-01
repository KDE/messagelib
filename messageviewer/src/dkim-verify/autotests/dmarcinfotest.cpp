/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcinfotest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dmarcinfo.h"
#include <QTest>
QTEST_GUILESS_MAIN(DMARCInfoTest)

DMARCInfoTest::DMARCInfoTest(QObject *parent)
    : QObject(parent)
{
}

void DMARCInfoTest::shouldHaveDefaultValues()
{
    MessageViewer::DMARCInfo info;
    QVERIFY(info.version().isEmpty());
    QVERIFY(info.adkim().isEmpty());
    QVERIFY(info.policy().isEmpty());
    QCOMPARE(info.percentage(), -1);
    QVERIFY(info.subDomainPolicy().isEmpty());
}

void DMARCInfoTest::shouldTestExtractDkimKeyRecord()
{
    QFETCH(QString, dkimstr);
    QFETCH(MessageViewer::DMARCInfo, dkiminfo);
    QFETCH(bool, isValid);
    MessageViewer::DMARCInfo info;
    QCOMPARE(info.parseDMARC(dkimstr), isValid);
    if (isValid) {
        const bool isEqual = (info == dkiminfo);
        if (!isEqual) {
            qDebug() << " info" << info;
            qDebug() << " dkiminforesult" << dkiminfo;
        }
        QVERIFY(isEqual);
    }
}

void DMARCInfoTest::shouldTestExtractDkimKeyRecord_data()
{
    QTest::addColumn<QString>("dkimstr");
    QTest::addColumn<MessageViewer::DMARCInfo>("dkiminfo");
    QTest::addColumn<bool>("isValid");
    QTest::addRow("empty") << QString() << MessageViewer::DMARCInfo() << false;
    MessageViewer::DMARCInfo info;
    info.setVersion(u"DMARC1"_s);
    info.setAdkim(u"r"_s);
    info.setPolicy(u"reject"_s);
    QTest::addRow("google.com") << u"v=DMARC1; p=reject; rua=mailto:mailauth-reports@google.com"_s << info << true;

    MessageViewer::DMARCInfo info2;
    info2.setVersion(u"DMARC1"_s);
    info2.setAdkim(u"r"_s);
    info2.setPolicy(u"reject"_s);
    info2.setPercentage(100);
    QTest::addRow("yahoo.com") << u"v=DMARC1; p=reject; pct=100; rua=mailto:dmarc_y_rua@yahoo.com;"_s << info2 << true;

    MessageViewer::DMARCInfo info3;
    info3.setVersion(u"DMARC1"_s);
    info3.setAdkim(u"r"_s);
    info3.setPolicy(u"none"_s);
    info3.setSubDomainPolicy(u"none"_s);
    QTest::addRow("intel.com") << u"v=DMARC1;p=none;sp=none;fo=1;rua=mailto:dmarc.notification@intel.com"_s << info3 << true;

    MessageViewer::DMARCInfo info4;
    info4.setVersion(u"DMARC1"_s);
    info4.setAdkim(u"s"_s);
    info4.setPolicy(u"reject"_s);
    QTest::addRow("strict adkim") << u"v=DMARC1; adkim=s; p=reject"_s << info4 << true;

    QTest::addRow("invalid adkim") << u"v=DMARC1; adkim=x; p=reject"_s << MessageViewer::DMARCInfo() << false;
    QTest::addRow("invalid pct") << u"v=DMARC1; p=reject; pct=abc"_s << MessageViewer::DMARCInfo() << false;
}

#include "moc_dmarcinfotest.cpp"
