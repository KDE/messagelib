/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoconvertertest.h"
#include "dkim-verify/dkimauthenticationstatusinfo.h"
#include "dkim-verify/dkimauthenticationstatusinfoconverter.h"
#include "dkim-verify/dkimchecksignaturejob.h"
#include <QTest>

using namespace Qt::Literals::StringLiterals;

QTEST_GUILESS_MAIN(DKIMAuthenticationStatusInfoConverterTest)

DKIMAuthenticationStatusInfoConverterTest::DKIMAuthenticationStatusInfoConverterTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMAuthenticationStatusInfoConverterTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMAuthenticationStatusInfoConverter status;
    QVERIFY(status.convert().isEmpty());
    QCOMPARE(status.statusInfo(), MessageViewer::DKIMAuthenticationStatusInfo());
}

void DKIMAuthenticationStatusInfoConverterTest::shouldConvertSoftFailFromAuthenticationStatus_data()
{
    QTest::addColumn<QString>("resultKeyword");

    QTest::addRow("softfail lowercase") << u"softfail"_s;
    QTest::addRow("softfail mixed case") << u"SoftFail"_s;
}

void DKIMAuthenticationStatusInfoConverterTest::shouldConvertSoftFailFromAuthenticationStatus()
{
    QFETCH(QString, resultKeyword);

    MessageViewer::DKIMAuthenticationStatusInfo statusInfo;
    MessageViewer::DKIMAuthenticationStatusInfo::AuthStatusInfo authStatusInfo;
    authStatusInfo.method = u"spf"_s;
    authStatusInfo.result = resultKeyword;
    authStatusInfo.reason = u"domain does not designate this host"_s;

    statusInfo.setListAuthStatusInfo({authStatusInfo});

    MessageViewer::DKIMAuthenticationStatusInfoConverter converter;
    converter.setStatusInfo(statusInfo);

    const auto converted = converter.convert();
    QCOMPARE(converted.size(), 1);

    const auto &entry = converted.at(0);
    QCOMPARE(entry.method, MessageViewer::DKIMCheckSignatureJob::AuthenticationMethod::Spf);
    QCOMPARE(entry.status, MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
    QCOMPARE(entry.infoResult, u"softfail"_s);
    QCOMPARE(entry.errorStr, u"domain does not designate this host"_s);
}

#include "moc_dkimauthenticationstatusinfoconvertertest.cpp"
