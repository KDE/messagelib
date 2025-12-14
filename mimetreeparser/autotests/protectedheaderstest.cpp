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
    otp.parseObjectTree(originalMessage.get());

    // test overwrite
    QCOMPARE(nodeHelper.mailHeaderAsBase("from", originalMessage.get())->asUnicodeString(), u"you@example.com"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("to", originalMessage.get())->asUnicodeString(), u"me@example.com"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("subject", originalMessage.get())->asUnicodeString(), u"hidden subject"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("cc", originalMessage.get())->asUnicodeString(), u"cc@example.com"_s);
    QCOMPARE(nodeHelper.dateHeader(originalMessage.get()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("message-id", originalMessage.get())->asUnicodeString(), u"<myhiddenreference@me>"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("references", originalMessage.get())->asUnicodeString(), u"<hiddenreference@hidden>"_s);
    QCOMPARE(nodeHelper.mailHeaderAsBase("in-reply-to", originalMessage.get())->asUnicodeString(), u"<hiddenreference@hidden>"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("invalid", originalMessage.get()), nullptr);

    // test envelope headers
    QCOMPARE(nodeHelper.mailHeaderAsBase("user-agent", originalMessage.get())->asUnicodeString(),
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
    otp.parseObjectTree(originalMessage.get());

    // test overwrite
    QCOMPARE(nodeHelper.headers("from", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("from", originalMessage.get())[0]->asUnicodeString(), u"you@example.com"_s);
    QCOMPARE(nodeHelper.headers("to", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("to", originalMessage.get())[0]->asUnicodeString(), u"me@example.com"_s);
    QCOMPARE(nodeHelper.headers("subject", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("subject", originalMessage.get())[0]->asUnicodeString(), u"hidden subject"_s);
    QCOMPARE(nodeHelper.headers("cc", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("cc", originalMessage.get())[0]->asUnicodeString(), u"cc@example.com"_s);
    QCOMPARE(nodeHelper.headers("date", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.dateHeader(originalMessage.get()), QDateTime(QDate(2018, 1, 2), QTime(3, 4, 5)));

    // test protected only headers
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("message-id", originalMessage.get())[0]->asUnicodeString(), u"<myhiddenreference@me>"_s);
    QCOMPARE(nodeHelper.headers("references", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("references", originalMessage.get())[0]->asUnicodeString(), u"<hiddenreference@hidden>"_s);
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("in-reply-to", originalMessage.get())[0]->asUnicodeString(), u"<hiddenreference@hidden>"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.headers("invalid", originalMessage.get()).isEmpty(), true);

    // test envelope headers
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.get()).size(), 1);
    QCOMPARE(nodeHelper.headers("user-agent", originalMessage.get())[0]->asUnicodeString(),
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
    otp.parseObjectTree(originalMessage.get());

    // test overwrite
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("from", originalMessage.get())->displayNames(), QStringList() << u"you@example.com"_s);
    QVERIFY(nodeHelper.mailHeaderAsAddressList("subject", originalMessage.get()));

    // test protected only headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("message-id", originalMessage.get())->displayNames(), QStringList() << u"myhiddenreference@me"_s);

    // test non-existing headers
    QCOMPARE(nodeHelper.mailHeaderAsAddressList("invalid", originalMessage.get()), nullptr);

    // test envelope headers
    QVERIFY(nodeHelper.mailHeaderAsAddressList("user-agent", originalMessage.get()));
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
    otp.parseObjectTree(originalMessage.get());

    // test overwrite
    QCOMPARE(nodeHelper.hasMailHeader("from", originalMessage.get()), true);

    // test protected only headers
    QCOMPARE(nodeHelper.hasMailHeader("message-id", originalMessage.get()), true);

    // test non-existing headers
    QCOMPARE(nodeHelper.hasMailHeader("invalid", originalMessage.get()), false);

    // test envelope headers
    QCOMPARE(nodeHelper.hasMailHeader("user-agent", originalMessage.get()), true);
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
    otp.parseObjectTree(originalMessage.get());

    // test overwrite
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("from", originalMessage.get())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, messagePartVector);
    }

    // test protected only headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("message-id", originalMessage.get())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, messagePartVector);
    }

    // test non-existing headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("invalid", originalMessage.get())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, QStringList());
    }

    // test envelope headers
    {
        QStringList actual;
        for (const auto &mp : nodeHelper.messagePartsOfMailHeader("user-agent", originalMessage.get())) {
            actual << QString::fromLatin1(mp->metaObject()->className());
        }
        QCOMPARE(actual, QStringList());
    }
}

#include "moc_protectedheaderstest.cpp"
