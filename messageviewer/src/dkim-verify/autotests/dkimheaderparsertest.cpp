/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimheaderparsertest.h"
#include "dkim-verify/dkimheaderparser.h"
#include <QTest>

QTEST_GUILESS_MAIN(DKIMHeaderParserTest)
DKIMHeaderParserTest::DKIMHeaderParserTest(QObject *parent)
    : QObject(parent)
{
}

void DKIMHeaderParserTest::shouldHaveDefaultValues()
{
    MessageViewer::DKIMHeaderParser parser;
    QVERIFY(parser.head().isEmpty());
    QVERIFY(parser.headerType(QStringLiteral("bla")).isEmpty());
    QVERIFY(!parser.wasAlreadyParsed());
}

void DKIMHeaderParserTest::shouldChangeWasAlreadyParsedBoolean()
{
    MessageViewer::DKIMHeaderParser parser;
    parser.setHead(QByteArray());
    parser.parse();
    QVERIFY(parser.wasAlreadyParsed());
}

void DKIMHeaderParserTest::shouldAssignElement()
{
    MessageViewer::DKIMHeaderParser parser;
    parser.setHead(QByteArray("blobli"));
    parser.parse();
    QVERIFY(parser.wasAlreadyParsed());

    MessageViewer::DKIMHeaderParser parser2 = parser;
    QCOMPARE(parser2, parser);
}

#include "moc_dkimheaderparsertest.cpp"
