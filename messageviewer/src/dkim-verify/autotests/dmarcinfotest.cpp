/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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
}
