/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dmarcinfotest.h"
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
    info.setVersion(QStringLiteral("DMARC1"));
    info.setAdkim(QStringLiteral("r"));
    info.setPolicy(QStringLiteral("reject"));
    QTest::addRow("google.com") << QStringLiteral("v=DMARC1; p=reject; rua=mailto:mailauth-reports@google.com") << info << true;

    MessageViewer::DMARCInfo info2;
    info2.setVersion(QStringLiteral("DMARC1"));
    info2.setAdkim(QStringLiteral("r"));
    info2.setPolicy(QStringLiteral("reject"));
    QTest::addRow("yahoo.com") << QStringLiteral("v=DMARC1; p=reject; pct=100; rua=mailto:dmarc_y_rua@yahoo.com;") << info2 << true;

    MessageViewer::DMARCInfo info3;
    info3.setVersion(QStringLiteral("DMARC1"));
    info3.setAdkim(QStringLiteral("r"));
    info3.setPolicy(QStringLiteral("none"));
    info3.setSubDomainPolicy(QStringLiteral("none"));
    QTest::addRow("intel.com") << QStringLiteral("v=DMARC1;p=none;sp=none;fo=1;rua=mailto:dmarc.notification@intel.com") << info3 << true;
}

#include "moc_dmarcinfotest.cpp"
