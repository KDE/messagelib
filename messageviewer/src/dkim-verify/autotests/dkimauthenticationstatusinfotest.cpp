/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfotest.h"
using namespace Qt::Literals::StringLiterals;

#include "dkim-verify/dkimauthenticationstatusinfo.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMAuthenticationStatusInfoTest)

DKIMAuthenticationStatusInfoTest::DKIMAuthenticationStatusInfoTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMAuthenticationStatusInfoTest::shouldHaveDefaultValue()
{
    MessageViewer::DKIMAuthenticationStatusInfo info;
    QVERIFY(info.authservId().isEmpty());
    QCOMPARE(info.authVersion(), -1);
    QVERIFY(info.reasonSpec().isEmpty());
    QVERIFY(info.listAuthStatusInfo().isEmpty());
}

void DKIMAuthenticationStatusInfoTest::shouldParseKey()
{
    QFETCH(QString, key);
    QFETCH(MessageViewer::DKIMAuthenticationStatusInfo, result);
    QFETCH(bool, relaxingParsing);
    QFETCH(bool, success);
    QBENCHMARK {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        const bool val = info.parseAuthenticationStatus(key, relaxingParsing);
        QCOMPARE(val, success);
        const bool compareResult = result == info;
        if (!compareResult) {
            qDebug() << "parse info: " << info;
            qDebug() << "expected: " << result;
        }
        QVERIFY(compareResult);
    }
    //    Before
    //        PASS   : DKIMAuthenticationStatusInfoTest::initTestCase()
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldHaveDefaultValue()
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(empty)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"empty":
    //             0.81 msecs per iteration (total: 52, iterations: 64)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(test1)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"test1":
    //             18 msecs per iteration (total: 72, iterations: 4)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(none)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"none":
    //             2.0 msecs per iteration (total: 65, iterations: 32)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(none2)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"none2":
    //             2.0 msecs per iteration (total: 66, iterations: 32)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(reason)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"reason":
    //             13 msecs per iteration (total: 52, iterations: 4)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(reason2)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"reason2":
    //             12 msecs per iteration (total: 97, iterations: 8)
    //        PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(gmails)
    //        RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"gmails":
    //             7.0 msecs per iteration (total: 56, iterations: 8)
    //        PASS   : DKIMAuthenticationStatusInfoTest::cleanupTestCase()

    //    AFTER
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"empty":
    //     0.0089 msecs per iteration (total: 73, iterations: 8192)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(test1)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"test1":
    //     0.23 msecs per iteration (total: 59, iterations: 256)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(none)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"none":
    //     0.023 msecs per iteration (total: 96, iterations: 4096)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(none2)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"none2":
    //     0.023 msecs per iteration (total: 96, iterations: 4096)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(reason)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"reason":
    //     0.16 msecs per iteration (total: 83, iterations: 512)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(reason2)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"reason2":
    //     0.14 msecs per iteration (total: 75, iterations: 512)
    // PASS   : DKIMAuthenticationStatusInfoTest::shouldParseKey(gmails)
    // RESULT : DKIMAuthenticationStatusInfoTest::shouldParseKey():"gmails":
    //     0.085 msecs per iteration (total: 88, iterations: 1024)
    // PASS   : DKIMAuthenticationStatusInfoTest::cleanupTestCase()
}

void DKIMAuthenticationStatusInfoTest::shouldParseKey_data()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<MessageViewer::DKIMAuthenticationStatusInfo>("result");
    QTest::addColumn<bool>("relaxingParsing");
    QTest::addColumn<bool>("success");

    QTest::addRow("empty") << QString() << MessageViewer::DKIMAuthenticationStatusInfo() << false << false;
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"in68.mail.ovh.net"_s);
        QList<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
        property.method = u"dkim"_s;
        property.result = u"pass"_s;
        property.methodVersion = 1;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"d"_s;
            prop.value = u"kde.org"_s;
            property.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"i"_s;
            prop.value = u"@kde.org"_s;
            property.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"b"_s;
            prop.value = u"\"GMG2ucPx\""_s;
            property.header.append(prop);
        }
        lst.append(property);

        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
        property2.method = u"dkim"_s;
        property2.result = u"pass"_s;
        property2.methodVersion = 1;

        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"d"_s;
            prop.value = u"kde.org"_s;
            property2.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"i"_s;
            prop.value = u"@kde.org"_s;
            property2.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = u"b"_s;
            prop.value = u"\"I3t3p7Up\""_s;
            property2.header.append(prop);
        }
        lst.append(property2);

        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property3;
        property3.method = u"dkim-atps"_s;
        property3.result = u"neutral"_s;
        property3.methodVersion = 1;
        lst.append(property3);

        info.setListAuthStatusInfo(lst);
        QTest::addRow("test1") << QStringLiteral(
            "in68.mail.ovh.net; dkim=pass (2048-bit key; unprotected) header.d=kde.org header.i=@kde.org header.b=\"GMG2ucPx\"; dkim=pass (2048-bit key; "
            "unprotected) header.d=kde.org header.i=@kde.org header.b=\"I3t3p7Up\"; dkim-atps=neutral")
                               << info << false << true;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"example.org"_s);

        QTest::addRow("none") << u"example.org 1; none;"_s << info << false << false;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"example.org"_s);

        QTest::addRow("none2") << u"example.org 1; none"_s << info << false << false;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"example.com"_s);

        QList<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = u"dkim"_s;
            property.result = u"pass"_s;
            property.reason = u"good signature"_s;
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = u"i"_s;
                prop.value = u"@mail-router.example.net"_s;
                property.header.append(prop);
            }
            lst.append(property);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
            property2.method = u"dkim"_s;
            property2.result = u"fail"_s;
            property2.reason = u"bad signature"_s;
            property2.methodVersion = 1;

            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = u"i"_s;
                prop.value = u"@newyork.example.com"_s;
                property2.header.append(prop);
            }
            lst.append(property2);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("reason") << QStringLiteral(
            "example.com; dkim=pass reason=\"good signature\" header.i=@mail-router.example.net; dkim=fail reason=\"bad signature\" "
            "header.i=@newyork.example.com;")
                                << info << false << true;
    }

    // It will failed. Fix it
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"example.com"_s);

        QList<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = u"dkim"_s;
            property.result = u"pass"_s;
            property.reason = u"good signature"_s;
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = u"i"_s;
                prop.value = u"@mail-router.example.net"_s;
                property.header.append(prop);
            }
            lst.append(property);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
            property2.method = u"dkim"_s;
            property2.result = u"fail"_s;
            property2.reason = u"bad signature"_s;
            property2.methodVersion = 1;

            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = u"i"_s;
                prop.value = u"@newyork.example.com"_s;
                property2.header.append(prop);
            }
            lst.append(property2);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("reason2") << QStringLiteral(
            "example.com; dkim=pass reason=\"good signature\" header.i=@mail-router.example.net; dkim=fail reason=\"bad signature\" "
            "header.i=@newyork.example.com")
                                 << info << true << true;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(u"letterbox.kde.org"_s);
        QList<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = u"dmarc"_s;
            property.result = u"pass"_s;
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = u"from"_s;
                prop.value = u"gmail.com"_s;
                property.header.append(prop);
            }
            lst.append(property);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("gmails") << u"letterbox.kde.org; dmarc=pass (p=none dis=none) header.from=gmail.com\r\n"_s << info << true << true;
    }
}

#include "moc_dkimauthenticationstatusinfotest.cpp"
