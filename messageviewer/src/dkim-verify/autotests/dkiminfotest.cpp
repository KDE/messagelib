/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkiminfotest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkiminfo.h"
#include <QDebug>
#include <QTest>

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
    QString val = QStringLiteral(
        "a=rsa-sha1; q=dns; d=example.com; i=user@eng.example.com; s=jun2005.eng; c=relaxed/simple; t=1117574938; x=1118006938; h=from:to:subject:date; "
        "b=dzdVyOfAKCdLXdJOc9G2q8LoXSlEniSbav+yuU4zGeeruD00lszZVoG4ZHRNiYzR");
    MessageViewer::DKIMInfo info1;
    info1.setVersion(1);
    info1.setQuery(u"dns"_s);
    info1.setDomain(u"example.com"_s);
    info1.setSigningAlgorithm(u"rsa"_s);
    info1.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha1);
    info1.setBodyCanonization(MessageViewer::DKIMInfo::Simple);
    info1.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info1.setSignatureTimeStamp(1117574938);
    info1.setExpireTime(1118006938);
    info1.setSelector(u"jun2005.eng"_s);
    info1.setAgentOrUserIdentifier(u"user@eng.example.com"_s);
    info1.setIDomain(u"eng.example.com"_s);
    info1.setSignature(u"dzdVyOfAKCdLXdJOc9G2q8LoXSlEniSbav+yuU4zGeeruD00lszZVoG4ZHRNiYzR"_s);
    info1.setListSignedHeader(QStringList({u"from"_s, u"to"_s, QStringLiteral("subject"), QStringLiteral("date")}));
    QTest::addRow("test1") << val << info1 << true;

    val = QStringLiteral(
        "v=1; a=rsa-sha256; c=relaxed/relaxed; d=example.com; "
        "h=message-id:content-type:content-type:user-agent:content-disposition:content-transfer-encoding:mime-version:reply-to:references:in-reply-to:subject:"
        "subject"
        ":list-unsubscribe:list-id:from:from:date:date; s=dkim; t=1569945303; x=1570809304; bh=/TC3U+LlxLH3YGxhC7qIHK8PzGl6Zx/8P6OGhDWrcWs=; "
        "b=w4GIjJb/+yEZvzvlw9yIPiuk/eKxAGSKL4WNmgC4D3V9fNyuVOV"
        "IH06PzqCuU/NwLas3SdAvd3VbTCObAb38KXeXkO7meeyCoR+kDsFzHpSAUg7+IRkeDR+RmarFjXwZAtoX3OMsB8euEprhS9fgGupxCWxwu6VGKJgt3Yu3/cI=");
    MessageViewer::DKIMInfo info2;
    info2.setVersion(1);
    info2.setQuery(u"dns/txt"_s);
    info2.setDomain(u"example.com"_s);
    info2.setIDomain(u"example.com"_s);
    info2.setAgentOrUserIdentifier(u"@example.com"_s);
    info2.setSigningAlgorithm(u"rsa"_s);
    info2.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha256);
    info2.setBodyCanonization(MessageViewer::DKIMInfo::Relaxed);
    info2.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info2.setSignatureTimeStamp(1569945303);
    info2.setExpireTime(1570809304);
    info2.setSelector(u"dkim"_s);
    info2.setSignature(
        QStringLiteral("w4GIjJb/+yEZvzvlw9yIPiuk/eKxAGSKL4WNmgC4D3V9fNyuVOVIH06PzqCuU/"
                       "NwLas3SdAvd3VbTCObAb38KXeXkO7meeyCoR+kDsFzHpSAUg7+IRkeDR+RmarFjXwZAtoX3OMsB8euEprhS9fgGupxCWxwu6VGKJgt3Yu3/cI="));
    info2.setBodyHash(u"/TC3U+LlxLH3YGxhC7qIHK8PzGl6Zx/8P6OGhDWrcWs="_s);
    info2.setListSignedHeader(QStringList({u"message-id"_s,
                                           u"content-type"_s,
                                           u"content-type"_s,
                                           u"user-agent"_s,
                                           u"content-disposition"_s,
                                           u"content-transfer-encoding"_s,
                                           u"mime-version"_s,
                                           u"reply-to"_s,
                                           u"references"_s,
                                           u"in-reply-to"_s,
                                           u"subject"_s,
                                           u"subject"_s,
                                           u"list-unsubscribe"_s,
                                           u"list-id"_s,
                                           u"from"_s,
                                           u"from"_s,
                                           u"date"_s,
                                           u"date"_s}));
    QTest::addRow("test2") << val << info2 << true;

    val = QStringLiteral(
        "v=1; a=rsa-sha1; c=relaxed; d=abonnement.radins.com; "
        "h=message-id:list-unsubscribe:from:to:reply-to:content-type:subject:content-transfer-encoding:mime-version:date; s=selector1; "
        "bh=vyAg5eFfq019WlDt9csu4bJMC54=; "
        "b=ABKgPqPe/MOGdgR2TJuiVNTLugsL8q/+ky/JxOxwZxnsPbtFnyJ+Y7Gk8bfcBL9myKPNqe7bU6Uy4IiNptn+v34rhVApm6ccoc44UXe/2A5D+6CPJHjFyf/ggjgF/"
        "BtQGYoMeQwj2+F4+QRxHSPldAcWqLCwlcRN25nPgiSAvWg=");
    MessageViewer::DKIMInfo info3;
    info3.setVersion(1);
    info3.setQuery(u"dns/txt"_s);
    info3.setDomain(u"abonnement.radins.com"_s);
    info3.setIDomain(u"abonnement.radins.com"_s);
    info3.setAgentOrUserIdentifier(u"@abonnement.radins.com"_s);
    info3.setSigningAlgorithm(u"rsa"_s);
    info3.setHashingAlgorithm(MessageViewer::DKIMInfo::HashingAlgorithmType::Sha1);
    info3.setBodyCanonization(MessageViewer::DKIMInfo::Simple);
    info3.setHeaderCanonization(MessageViewer::DKIMInfo::Relaxed);
    info3.setSelector(u"selector1"_s);
    info3.setSignature(
        QStringLiteral("ABKgPqPe/MOGdgR2TJuiVNTLugsL8q/+ky/JxOxwZxnsPbtFnyJ+Y7Gk8bfcBL9myKPNqe7bU6Uy4IiNptn+v34rhVApm6ccoc44UXe/2A5D+6CPJHjFyf/ggjgF/"
                       "BtQGYoMeQwj2+F4+QRxHSPldAcWqLCwlcRN25nPgiSAvWg="));
    info3.setBodyHash(u"vyAg5eFfq019WlDt9csu4bJMC54="_s);
    info3.setListSignedHeader(QStringList({u"message-id"_s,
                                           u"list-unsubscribe"_s,
                                           u"from"_s,
                                           u"to"_s,
                                           u"reply-to"_s,
                                           u"content-type"_s,
                                           u"subject"_s,
                                           u"content-transfer-encoding"_s,
                                           u"mime-version"_s,
                                           u"date"_s}));
    QTest::addRow("test3") << val << info3 << true;
}

void DKIMInfoTest::shouldTestExtractDkimInfo()
{
    QFETCH(QString, dkimstr);
    QFETCH(MessageViewer::DKIMInfo, dkiminforesult);
    QFETCH(bool, isValid);

    if (QString::fromLatin1(QTest::currentDataTag()) == u"test1"_s) {
        QSKIP("FIXME: Failing for too long");
    }

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

#include "moc_dkiminfotest.cpp"
