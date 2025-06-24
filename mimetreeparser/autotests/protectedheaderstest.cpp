/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "protectedheaderstest.h"
using namespace Qt::Literals::StringLiterals;

#include "setupenv.h"
#include "util.h"

#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QTest>

using namespace MimeTreeParser;

QTEST_MAIN(ProtectedHeadersTest)

void ProtectedHeadersTest::initTestCase()
{
    Test::setupEnv();
    qputenv("TZ", "GMT");
}

void ProtectedHeadersTest::testMailHeaderAsBase_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << u"openpgp-encrypted-protected-headers.mbox"_s;
    QTest::newRow("signed") << u"openpgp-signed-protected-headers.mbox"_s;
    QTest::newRow("encrypted+signed") << u"openpgp-encrypted+signed-protected-headers.mbox"_s;
}

void ProtectedHeadersTest::testMailHeaderAsBase()
{
    QFETCH(QString, mailFileName);
    auto originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    // test overwrite
    QCOMPARE(nodeHelper.mailHeaderAsBase("from", originalMessage.data())->asUnicodeString(), u"you@example.com"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("to", originalMessage.data())->asUnicodeString(), u"me@example.com"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("subject", originalMessage.data())->asUnicodeString(), u"hidden subject"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("cc", originalMessage.data())->asUnicodeString(), u"cc@example.com"_s);
    QCOMPARE(nodeHelper.dateHeader(originalMessage.data()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("message-id", originalMessage.data())->asUnicodeString(), u"<myhiddenreference@me>"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("references", originalMessage.data())->asUnicodeString(), u"<hiddenreference@hidden>"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("in-reply-to", originalMessage.data())->asUnicodeString(), u"<hiddenreference@hidden>"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("invalid", originalMessage.data()), nullptr);

    // test envelope headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("user-agent", originalMessage.data())->asUnicodeString(),
             u"KMail/4.6 pre (Linux/2.6.34-rc2-2-default; KDE/4.5.60; x86_64; ; )"_s);
}

void ProtectedHeadersTest::testHeaders_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << u"openpgp-encrypted-protected-headers.mbox"_s;
    QTest::newRow("signed") << u"openpgp-signed-protected-headers.mbox"_s;
    QTest::newRow("encrypted+signed") << u"openpgp-encrypted+signed-protected-headers.mbox"_s;
}

void ProtectedHeadersTest::testHeaders()
{
    QFETCH(QString, mailFileName);
    auto originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    // test overwrite
    QCOMPARE(nodeHelper.headers("from", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("from", originalMessage.data())[0]->asUnicodeString(), u"you@example.com"_s);
    QCOMPARE(nodeHelper.headers("to", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("to", originalMessage.data())[0]->asUnicodeString(), u"me@example.com"_s);
    QCOMPARE(nodeHelper.headers("subject", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("subject", originalMessage.data())[0]->asUnicodeString(), u"hidden subject"_s);
    QCOMPARE(nodeHelper.headers("cc", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("cc", originalMessage.data())[0]->asUnicodeString(), u"cc@example.com"_s);
    QCOMPARE(nodeHelper.headers("date", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.dateHeader(originalMessage.data()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.data())[0]->asUnicodeString(), u"<myhiddenreference@me>"_s);
    QCOMPARE(nodeHelper.headers("references", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("references", originalMessage.data())[0]->asUnicodeString(), u"<hiddenreference@hidden>"_s);
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.data())[0]->asUnicodeString(), u"<hiddenreference@hidden>"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.headers("invalid", originalMessage.data()).isEmpty(), true);

    // test envelope headers
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.data())[0]->asUnicodeString(),
             u"KMail/4.6 pre (Linux/2.6.34-rc2-2-default; KDE/4.5.60; x86_64; ; )"_s);
}

void ProtectedHeadersTest::testMailHeaderAsAddresslist_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << u"openpgp-encrypted-protected-headers.mbox"_s;
    QTest::newRow("signed") << u"openpgp-signed-protected-headers.mbox"_s;
    QTest::newRow("encrypted+signed") << u"openpgp-encrypted+signed-protected-headers.mbox"_s;
}

void ProtectedHeadersTest::testMailHeaderAsAddresslist()
{
    QFETCH(QString, mailFileName);
    auto originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    // test overwrite
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("from", originalMessage.data())->displayNames(), QStringList() << u"you@example.com"_s);
    QVERIFY(nodeHelper.mailHeaderAsAddressList("subject", originalMessage.data()));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("message-id", originalMessage.data())->displayNames(), QStringList() << u"myhiddenreference@me"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("invalid", originalMessage.data()), nullptr);

    // test envelope headers
    QVERIFY(nodeHelper.mailHeaderAsAddressList("user-agent", originalMessage.data()));
}

void ProtectedHeadersTest::testhasMailHeader_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << u"openpgp-encrypted-protected-headers.mbox"_s;
    QTest::newRow("signed") << u"openpgp-signed-protected-headers.mbox"_s;
    QTest::newRow("encrypted+signed") << u"openpgp-encrypted+signed-protected-headers.mbox"_s;
}

void ProtectedHeadersTest::testhasMailHeader()
{
    QFETCH(QString, mailFileName);
    auto originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    // test overwrite
    QCOMPARE(nodeHelper.hasMailHeader("from", originalMessage.data()), true);

    // test protected only headers
    QCOMPARE(nodeHelper.hasMailHeader("message-id", originalMessage.data()), true);

    // test non-existing headers
    QCOMPARE(nodeHelper.hasMailHeader("invalid", originalMessage.data()), false);

    // test envelope headers
    QCOMPARE(nodeHelper.hasMailHeader("user-agent", originalMessage.data()), true);
}

void ProtectedHeadersTest::testMessagePartsOfMailHeader_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QStringList>("messagePartVector");

    QStringList encryptedPart;
    QStringList signedPart;
    encryptedPart << u"MimeTreeParser::EncryptedMessagePart"_s;
    signedPart << u"MimeTreeParser::SignedMessagePart"_s;

    QTest::newRow("encrypted") << u"openpgp-encrypted-protected-headers.mbox"_s << encryptedPart;
    QTest::newRow("signed") << u"openpgp-signed-protected-headers.mbox"_s << signedPart;
    QTest::newRow("encrypted+signed") << u"openpgp-encrypted+signed-protected-headers.mbox"_s << encryptedPart;
}

void ProtectedHeadersTest::testMessagePartsOfMailHeader()
{
    QFETCH(QString, mailFileName);
    QFETCH(QStringList, messagePartVector);

    auto originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    // test overwrite
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("from", originalMessage.data())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, messagePartVector);
    }

    // test protected only headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("message-id", originalMessage.data())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, messagePartVector);
    }

    // test non-existing headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("invalid", originalMessage.data())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, QStringList());
    }

    // test envelope headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("user-agent", originalMessage.data())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, QStringList());
    }
}

#include "moc_protectedheaderstest.cpp"
