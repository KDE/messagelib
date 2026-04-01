/*
   SPDX-FileCopyrightText: 2026 Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "eaitest.h"

#include <KEmailAddress>
#include <KMime/Message>

#include <QTest>

using namespace Qt::Literals::StringLiterals;

QTEST_GUILESS_MAIN(EaiTest)

// An EAI message: RFC 2047 encoded display name plus a raw UTF-8 addr-spec
// localpart, as RFC 6532 permits and SMTPUTF8 (RFC 6531) requires.
static const QByteArray eaiMessage =
    "From: =?utf-8?q?Gr=C3=A5_katt?= <gr\xC3\xA5@example.com>\r\n"
    "To: test@example.com\r\n"
    "Subject: Test\r\n"
    "MIME-Version: 1.0\r\n"
    "Content-Type: text/plain; charset=utf-8\r\n"
    "\r\n"
    "Body\r\n";

// Demonstrates the encoding contract that viewer_p.cpp's raw-source view
// relies on: encodedContent() emits raw UTF-8 bytes, so fromUtf8() round-trips
// the addr-spec while fromLatin1() does not.
void EaiTest::testRawSourceEncoding()
{
    auto msg = std::make_shared<KMime::Message>();
    msg->setContent(eaiMessage);
    msg->parse();

    const QByteArray encoded = msg->encodedContent();

    QVERIFY(!QString::fromLatin1(encoded).contains(u"grå@example.com"));
    QVERIFY(QString::fromUtf8(encoded).contains(u"grå@example.com"));
}

// Demonstrates the encoding contract that headerstyle_util.cpp's xface()
// relies on: KEmailAddress::firstEmailAddress() returns raw bytes from the
// header, which are UTF-8 for an EAI address. The bytes are constructed by
// hand here so the test does not depend on kmime's emission policy.
void EaiTest::testAddressExtractionEncoding()
{
    const QByteArray headerBytes = "=?utf-8?q?Gr=C3=A5_katt?= <gr\xC3\xA5@example.com>";
    const QByteArray addrBytes = KEmailAddress::firstEmailAddress(headerBytes);

    QVERIFY(!QString::fromLatin1(addrBytes).contains(u"grå@example.com"));
    QCOMPARE(QString::fromUtf8(addrBytes), u"grå@example.com"_s);
}
