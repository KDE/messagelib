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
    QVERIFY(info.authVersion().isEmpty());
    QVERIFY(info.reasonSpec().isEmpty());
}

void DKIMAuthenticationStatusInfoTest::shouldParseKey()
{
    QFETCH(QString, key);
    QFETCH(QString, result);
    QFETCH(bool, success);
    MessageViewer::DKIMAuthenticationStatusInfo info;
    const bool val = info.parseAuthenticationStatus(key);
    QCOMPARE(val, success);
    //TODO test result.
}

void DKIMAuthenticationStatusInfoTest::shouldParseKey_data()
{
    QTest::addColumn<QString>("key");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("success");

    QTest::addRow("empty") << QString() << QString() << true;
}
