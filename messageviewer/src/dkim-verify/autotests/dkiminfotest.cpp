/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "dkiminfotest.h"
#include "dkim-verify/dkiminfo.h"
#include <QTest>
#include <QDebug>

QTEST_GUILESS_MAIN(DKIMInfoTest)

DKIMInfoTest::DKIMInfoTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMInfoTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMInfo info;
    QCOMPARE(info.version(), -1);

    QCOMPARE(info.hashingAlgorithm(), MessageViewer::DKIMInfo::HashingAlgorithmType::Any);
    QVERIFY(info.signingAlgorithm().isEmpty());
    QVERIFY(info.domain().isEmpty());
    QVERIFY(info.selector().isEmpty());
    QVERIFY(info.bodyHash().isEmpty());
    QVERIFY(info.listSignedHeader().isEmpty());
    QVERIFY(info.query().isEmpty());
    QCOMPARE(info.signatureTimeStamp(), -1);
    QCOMPARE(info.expireTime(), -1);
    QVERIFY(info.signature().isEmpty());
    QVERIFY(info.agentOrUserIdentifier().isEmpty());
    QCOMPARE(info.bodyLengthCount(), -1);
    QCOMPARE(info.headerCanonization(), MessageViewer::DKIMInfo::Unknown);
    QCOMPARE(info.bodyCanonization(), MessageViewer::DKIMInfo::Unknown);
    QVERIFY(info.copiedHeaderField().isEmpty());
    QVERIFY(info.iDomain().isEmpty());
}

void DKIMInfoTest::shouldTestExtractDkimInfo_data()
{
    QTest::addColumn<QString>("dkimstr");
    QTest::addColumn<MessageViewer::DKIMInfo>("dkiminforesult");
    QTest::addColumn<bool>("isValid");
    QTest::addRow("empty") << QString() << MessageViewer::DKIMInfo() << false;
    QString val = QStringLiteral("a=rsa-sha1; q=dns; d=example.com; i=user@eng.example.com; s=jun2005.eng; c=relaxed/simple; t=1117574938; x=1118006938; h=from:to:subject:date; b=dzdVyOfAKCdLXdJOc9G2q8LoXSlEniSbav+yuU4zGeeruD00lszZVoG4ZHRNiYzR");
    MessageViewer::DKIMInfo info1;
    info1.setVersion(1);
    info1.setQuery(QStringLiteral("dns"));
    info1.setDomain(QStringLiteral("example.com"));
    info1.setSigningAlgorithm(QStringLiteral("rsa"));
    info1.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha1);
    info1.setBodyCanonization(MessageViewer::DKIMInfo::Simple);
    info1.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info1.setSignatureTimeStamp(1117574938);
    info1.setExpireTime(1118006938);
    info1.setSelector(QStringLiteral("jun2005.eng"));
    info1.setAgentOrUserIdentifier(QStringLiteral("user"));
    info1.setIDomain(QStringLiteral("eng.example.com"));
    info1.setSignature(QStringLiteral("dzdVyOfAKCdLXdJOc9G2q8LoXSlEniSbav+yuU4zGeeruD00lszZVoG4ZHRNiYzR"));
    info1.setListSignedHeader(QStringList({QStringLiteral("from"), QStringLiteral("to"), QStringLiteral("subject"), QStringLiteral("date")}));
    QTest::addRow("test1") << val << info1 << true;

    val = QStringLiteral("v=1; a=rsa-sha256; c=relaxed/relaxed; d=example.com; h=message-id:content-type:content-type:user-agent:content-disposition:content-transfer-encoding:mime-version:reply-to:references:in-reply-to:subject:subject"
                         ":list-unsubscribe:list-id:from:from:date:date; s=dkim; t=1569945303; x=1570809304; bh=/TC3U+LlxLH3YGxhC7qIHK8PzGl6Zx/8P6OGhDWrcWs=; b=w4GIjJb/+yEZvzvlw9yIPiuk/eKxAGSKL4WNmgC4D3V9fNyuVOV"
                         "IH06PzqCuU/NwLas3SdAvd3VbTCObAb38KXeXkO7meeyCoR+kDsFzHpSAUg7+IRkeDR+RmarFjXwZAtoX3OMsB8euEprhS9fgGupxCWxwu6VGKJgt3Yu3/cI=");
    MessageViewer::DKIMInfo info2;
    info2.setVersion(1);
    info2.setQuery(QStringLiteral("dns/txt"));
    info2.setDomain(QStringLiteral("example.com"));
    info2.setIDomain(QStringLiteral("example.com"));
    info2.setAgentOrUserIdentifier(QStringLiteral("@example.com"));
    info2.setSigningAlgorithm(QStringLiteral("rsa"));
    info2.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha256);
    info2.setBodyCanonization(MessageViewer::DKIMInfo::Relaxed);
    info2.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info2.setSignatureTimeStamp(1569945303);
    info2.setExpireTime(1570809304);
    info2.setSelector(QStringLiteral("dkim"));
    info2.setSignature(QStringLiteral("w4GIjJb/+yEZvzvlw9yIPiuk/eKxAGSKL4WNmgC4D3V9fNyuVOVIH06PzqCuU/NwLas3SdAvd3VbTCObAb38KXeXkO7meeyCoR+kDsFzHpSAUg7+IRkeDR+RmarFjXwZAtoX3OMsB8euEprhS9fgGupxCWxwu6VGKJgt3Yu3/cI="));
    info2.setBodyHash(QStringLiteral("/TC3U+LlxLH3YGxhC7qIHK8PzGl6Zx/8P6OGhDWrcWs="));
    info2.setListSignedHeader(QStringList({QStringLiteral("message-id"), QStringLiteral("content-type"), QStringLiteral("content-type"),
                                           QStringLiteral("user-agent"), QStringLiteral("content-disposition"),
                                           QStringLiteral("content-transfer-encoding"), QStringLiteral("mime-version"),
                                           QStringLiteral("reply-to"), QStringLiteral("references"),
                                           QStringLiteral("in-reply-to"), QStringLiteral("subject"),
                                           QStringLiteral("subject"), QStringLiteral("list-unsubscribe"),
                                           QStringLiteral("list-id"), QStringLiteral("from"), QStringLiteral("from"),
                                           QStringLiteral("date"), QStringLiteral("date")}));
    QTest::addRow("test2") << val << info2 << true;

    val = QStringLiteral(
        "v=1; a=rsa-sha1; c=relaxed; d=abonnement.radins.com; h=message-id:list-unsubscribe:from:to:reply-to:content-type:subject:content-transfer-encoding:mime-version:date; s=selector1; bh=vyAg5eFfq019WlDt9csu4bJMC54=; b=ABKgPqPe/MOGdgR2TJuiVNTLugsL8q/+ky/JxOxwZxnsPbtFnyJ+Y7Gk8bfcBL9myKPNqe7bU6Uy4IiNptn+v34rhVApm6ccoc44UXe/2A5D+6CPJHjFyf/ggjgF/BtQGYoMeQwj2+F4+QRxHSPldAcWqLCwlcRN25nPgiSAvWg=");
    MessageViewer::DKIMInfo info3;
    info3.setVersion(1);
    info3.setQuery(QStringLiteral("dns/txt"));
    info3.setDomain(QStringLiteral("abonnement.radins.com"));
    info3.setIDomain(QStringLiteral("abonnement.radins.com"));
    info3.setAgentOrUserIdentifier(QStringLiteral("@abonnement.radins.com"));
    info3.setSigningAlgorithm(QStringLiteral("rsa"));
    info3.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha1);
    info3.setBodyCanonization(MessageViewer::DKIMInfo::Simple);
    info3.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info3.setSelector(QStringLiteral("selector1"));
    info3.setSignature(QStringLiteral("ABKgPqPe/MOGdgR2TJuiVNTLugsL8q/+ky/JxOxwZxnsPbtFnyJ+Y7Gk8bfcBL9myKPNqe7bU6Uy4IiNptn+v34rhVApm6ccoc44UXe/2A5D+6CPJHjFyf/ggjgF/BtQGYoMeQwj2+F4+QRxHSPldAcWqLCwlcRN25nPgiSAvWg="));
    info3.setBodyHash(QStringLiteral("vyAg5eFfq019WlDt9csu4bJMC54="));
    info3.setListSignedHeader(QStringList({QStringLiteral("message-id"), QStringLiteral("list-unsubscribe"),
                                           QStringLiteral("from"), QStringLiteral("to"), QStringLiteral("reply-to"), QStringLiteral("content-type"),
                                           QStringLiteral("subject"), QStringLiteral("content-transfer-encoding"), QStringLiteral("mime-version"), QStringLiteral("date")}));
    QTest::addRow("test3") << val << info3 << true;
}

void DKIMInfoTest::shouldTestExtractDkimInfo()
{
    QFETCH(QString, dkimstr);
    QFETCH(MessageViewer::DKIMInfo, dkiminforesult);
    QFETCH(bool, isValid);
    MessageViewer::DKIMInfo info;
    QCOMPARE(info.parseDKIM(dkimstr), isValid);
    if (isValid) {
        const bool isEqual = (info == dkiminforesult);
        if (!isEqual) {
            qDebug() << " info" << info;
            qDebug() << " dkiminforesult" << dkiminforesult;
        }
        QVERIFY(isEqual);
    }
}
