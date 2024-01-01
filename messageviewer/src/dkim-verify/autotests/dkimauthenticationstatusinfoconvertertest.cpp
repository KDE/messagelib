/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimauthenticationstatusinfoconvertertest.h"
#include "dkim-verify/dkimauthenticationstatusinfoconverter.h"
#include <QTest>
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

#include "moc_dkimauthenticationstatusinfoconvertertest.cpp"
