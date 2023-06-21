/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "protectedheaderstest.h"
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

    QTest::newRow("encrypted") << QStringLiteral("openpgp-encrypted-protected-headers.mbox");
    QTest::newRow("signed") << QStringLiteral("openpgp-signed-protected-headers.mbox");
    QTest::newRow("encrypted+signed") << QStringLiteral("openpgp-encrypted+signed-protected-headers.mbox");
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
    QCOMPARE(nodeHelper.mailHeaderAsBase("from", originalMessage.data())->asUnicodeString(), QStringLiteral("you@example.com"));
    QCOMPARE(nodeHelper.mailHeaderAsBase("to", originalMessage.data())->asUnicodeString(), QStringLiteral("me@example.com"));
    QCOMPARE(nodeHelper.mailHeaderAsBase("subject", originalMessage.data())->asUnicodeString(), QStringLiteral("hidden subject"));
    QCOMPARE(nodeHelper.mailHeaderAsBase("cc", originalMessage.data())->asUnicodeString(), QStringLiteral("cc@example.com"));
    QCOMPARE(nodeHelper.dateHeader(originalMessage.data()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("message-id", originalMessage.data())->asUnicodeString(), QStringLiteral("<myhiddenreference@me>"));
    QCOMPARE(nodeHelper.mailHeaderAsBase("references", originalMessage.data())->asUnicodeString(), QStringLiteral("<hiddenreference@hidden>"));
    QCOMPARE(nodeHelper.mailHeaderAsBase("in-reply-to", originalMessage.data())->asUnicodeString(), QStringLiteral("<hiddenreference@hidden>"));

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("invalid", originalMessage.data()), nullptr);

    // test envelope headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("user-agent", originalMessage.data())->asUnicodeString(),
             QStringLiteral("KMail/4.6 pre (Linux/2.6.34-rc2-2-default; KDE/4.5.60; x86_64; ; )"));
}

void ProtectedHeadersTest::testHeaders_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << QStringLiteral("openpgp-encrypted-protected-headers.mbox");
    QTest::newRow("signed") << QStringLiteral("openpgp-signed-protected-headers.mbox");
    QTest::newRow("encrypted+signed") << QStringLiteral("openpgp-encrypted+signed-protected-headers.mbox");
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
    QCOMPARE(nodeHelper.headers("from", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("you@example.com"));
    QCOMPARE(nodeHelper.headers("to", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("to", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("me@example.com"));
    QCOMPARE(nodeHelper.headers("subject", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("subject", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("hidden subject"));
    QCOMPARE(nodeHelper.headers("cc", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("cc", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("cc@example.com"));
    QCOMPARE(nodeHelper.headers("date", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.dateHeader(originalMessage.data()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("<myhiddenreference@me>"));
    QCOMPARE(nodeHelper.headers("references", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("references", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("<hiddenreference@hidden>"));
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.data())[0]->asUnicodeString(), QStringLiteral("<hiddenreference@hidden>"));

    // test non-existing headers
    QCOMPARE(nodeHelper.headers("invalid", originalMessage.data()).isEmpty(), true);

    // test envelope headers
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.data()).size(), 1);
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.data())[0]->asUnicodeString(),
             QStringLiteral("KMail/4.6 pre (Linux/2.6.34-rc2-2-default; KDE/4.5.60; x86_64; ; )"));
}

void ProtectedHeadersTest::testMailHeaderAsAddresslist_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << QStringLiteral("openpgp-encrypted-protected-headers.mbox");
    QTest::newRow("signed") << QStringLiteral("openpgp-signed-protected-headers.mbox");
    QTest::newRow("encrypted+signed") << QStringLiteral("openpgp-encrypted+signed-protected-headers.mbox");
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
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("from", originalMessage.data())->displayNames(), QStringList() << QStringLiteral("you@example.com"));
    QVERIFY(nodeHelper.mailHeaderAsAddressList("subject", originalMessage.data()));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("message-id", originalMessage.data())->displayNames(), QStringList() << QStringLiteral("myhiddenreference@me"));

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("invalid", originalMessage.data()), nullptr);

    // test envelope headers
    QVERIFY(nodeHelper.mailHeaderAsAddressList("user-agent", originalMessage.data()));
}

void ProtectedHeadersTest::testhasMailHeader_data()
{
    QTest::addColumn<QString>("mailFileName");

    QTest::newRow("encrypted") << QStringLiteral("openpgp-encrypted-protected-headers.mbox");
    QTest::newRow("signed") << QStringLiteral("openpgp-signed-protected-headers.mbox");
    QTest::newRow("encrypted+signed") << QStringLiteral("openpgp-encrypted+signed-protected-headers.mbox");
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
    encryptedPart << QStringLiteral("MimeTreeParser::EncryptedMessagePart");
    signedPart << QStringLiteral("MimeTreeParser::SignedMessagePart");

    QTest::newRow("encrypted") << QStringLiteral("openpgp-encrypted-protected-headers.mbox") << encryptedPart;
    QTest::newRow("signed") << QStringLiteral("openpgp-signed-protected-headers.mbox") << signedPart;
    QTest::newRow("encrypted+signed") << QStringLiteral("openpgp-encrypted+signed-protected-headers.mbox") << encryptedPart;
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
