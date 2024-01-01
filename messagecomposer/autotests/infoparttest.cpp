/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "infoparttest.h"
#include "../src/part/infopart.h"
#include <QTest>
InfoPartTest::InfoPartTest(QObject *parent)
    : QObject(parent)
{
}

InfoPartTest::~InfoPartTest() = default;

void InfoPartTest::shouldHaveDefaultValue()
{
    MessageComposer::InfoPart infopart;
    QCOMPARE(infopart.transportId(), 0);
    QVERIFY(!infopart.urgent());
    QVERIFY(infopart.from().isEmpty());
    QVERIFY(infopart.to().isEmpty());
    QVERIFY(infopart.cc().isEmpty());
    QVERIFY(infopart.bcc().isEmpty());
    QVERIFY(infopart.replyTo().isEmpty());
    QVERIFY(infopart.subject().isEmpty());
    QVERIFY(infopart.fcc().isEmpty());
    QVERIFY(infopart.userAgent().isEmpty());
    QVERIFY(infopart.inReplyTo().isEmpty());
    QVERIFY(infopart.references().isEmpty());
    QVERIFY(infopart.extraHeaders().isEmpty());
}

QTEST_MAIN(InfoPartTest)

#include "moc_infoparttest.cpp"
