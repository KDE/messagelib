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

#include "dkimauthenticationstatusinfotest.h"
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
        info.setAuthservId(QStringLiteral("in68.mail.ovh.net"));
        QVector<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
        property.method = QStringLiteral("dkim");
        property.result = QStringLiteral("pass");
        property.methodVersion = 1;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("d");
            prop.value = QLatin1String("kde.org");
            property.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("i");
            prop.value = QLatin1String("@kde.org");
            property.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("b");
            prop.value = QLatin1String("\"GMG2ucPx\"");
            property.header.append(prop);
        }
        lst.append(property);

        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
        property2.method = QStringLiteral("dkim");
        property2.result = QStringLiteral("pass");
        property2.methodVersion = 1;

        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("d");
            prop.value = QLatin1String("kde.org");
            property2.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("i");
            prop.value = QLatin1String("@kde.org");
            property2.header.append(prop);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
            prop.type = QLatin1String("b");
            prop.value = QLatin1String("\"I3t3p7Up\"");
            property2.header.append(prop);
        }
        lst.append(property2);

        MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property3;
        property3.method = QStringLiteral("dkim-atps");
        property3.result = QStringLiteral("neutral");
        property3.methodVersion = 1;
        lst.append(property3);

        info.setListAuthStatusInfo(lst);
        QTest::addRow("test1") << QStringLiteral("in68.mail.ovh.net; dkim=pass (2048-bit key; unprotected) header.d=kde.org header.i=@kde.org header.b=\"GMG2ucPx\"; dkim=pass (2048-bit key; unprotected) header.d=kde.org header.i=@kde.org header.b=\"I3t3p7Up\"; dkim-atps=neutral")
                               << info
                               << false
                               << true;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(QStringLiteral("example.org"));

        QTest::addRow("none") << QStringLiteral("example.org 1; none;")
                              << info
                              << false
                              << false;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(QStringLiteral("example.org"));

        QTest::addRow("none2") << QStringLiteral("example.org 1; none")
                               << info
                               << false
                               << false;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(QStringLiteral("example.com"));

        QVector<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = QStringLiteral("dkim");
            property.result = QStringLiteral("pass");
            property.reason = QStringLiteral("good signature");
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = QLatin1String("i");
                prop.value = QLatin1String("@mail-router.example.net");
                property.header.append(prop);
            }
            lst.append(property);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
            property2.method = QStringLiteral("dkim");
            property2.result = QStringLiteral("fail");
            property2.reason = QStringLiteral("bad signature");
            property2.methodVersion = 1;

            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = QLatin1String("i");
                prop.value = QLatin1String("@newyork.example.com");
                property2.header.append(prop);
            }
            lst.append(property2);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("reason") << QStringLiteral("example.com; dkim=pass reason=\"good signature\" header.i=@mail-router.example.net; dkim=fail reason=\"bad signature\" header.i=@newyork.example.com;")
                                << info
                                << false
                                << true;
    }

    //It will failed. Fix it
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(QStringLiteral("example.com"));

        QVector<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = QStringLiteral("dkim");
            property.result = QStringLiteral("pass");
            property.reason = QStringLiteral("good signature");
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = QLatin1String("i");
                prop.value = QLatin1String("@mail-router.example.net");
                property.header.append(prop);
            }
            lst.append(property);
        }
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property2;
            property2.method = QStringLiteral("dkim");
            property2.result = QStringLiteral("fail");
            property2.reason = QStringLiteral("bad signature");
            property2.methodVersion = 1;

            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = QLatin1String("i");
                prop.value = QLatin1String("@newyork.example.com");
                property2.header.append(prop);
            }
            lst.append(property2);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("reason2") << QStringLiteral("example.com; dkim=pass reason=\"good signature\" header.i=@mail-router.example.net; dkim=fail reason=\"bad signature\" header.i=@newyork.example.com")
                                 << info
                                 << true
                                 << true;
    }
    {
        MessageViewer::DKIMAuthenticationStatusInfo info;
        info.setAuthVersion(1);
        info.setAuthservId(QStringLiteral("letterbox.kde.org"));
        QVector<MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo> lst;
        {
            MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo property;
            property.method = QStringLiteral("dmarc");
            property.result = QStringLiteral("pass");
            property.methodVersion = 1;
            {
                MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo::Property prop;
                prop.type = QLatin1String("from");
                prop.value = QLatin1String("gmail.com");
                property.header.append(prop);
            }
            lst.append(property);
        }
        info.setListAuthStatusInfo(lst);

        QTest::addRow("gmails") << QStringLiteral("letterbox.kde.org; dmarc=pass (p=none dis=none) header.from=gmail.com\r\n")
                                << info
                                << true
                                << true;
    }
}
