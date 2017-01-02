/*
  Copyright (c) 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "unencryptedmessagetest.h"
#include "util.h"
#include "setupenv.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>

#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(UnencryptedMessageTest)

void UnencryptedMessageTest::initTestCase()
{
    Test::setupEnv();
}

void UnencryptedMessageTest::testMailWithoutEncryption()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox"));
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());
    QVERIFY(!nodeHelper.unencryptedMessage(originalMessage));
}

void UnencryptedMessageTest::testSignedForwardedOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("signed-forward-openpgp-signed-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::HtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "bla bla bla");   // The textual content doesn't include the encrypted encapsulated message by design
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgPartiallyEncrypted);
    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), MimeTreeParser::KMMsgFullySigned);

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QVERIFY(!unencryptedMessage);   // We must not invalidate the outer signature
}

void UnencryptedMessageTest::testForwardedOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("forward-openpgp-signed-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::HtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    emptySource.setAllowDecryption(true);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "bla bla bla");   // The textual content doesn't include the encrypted encapsulated message by design
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgPartiallyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), MimeTreeParser::KMMsgPartiallySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QVERIFY(unencryptedMessage.data());
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "multipart/mixed");
    QCOMPARE(unencryptedMessage->contents().size(), 2);
    QCOMPARE(unencryptedMessage->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().first()->decodedContent().data(), "bla bla bla");
    QCOMPARE(unencryptedMessage->contents().at(1)->contentType()->mimeType().data(), "message/rfc822");
    KMime::Message::Ptr encapsulated = unencryptedMessage->contents().at(1)->bodyAsMessage();
    QCOMPARE(encapsulated->contentType()->mimeType().data(), "multipart/signed");
    QCOMPARE(encapsulated->contents().size(), 2);
    QCOMPARE(encapsulated->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(encapsulated->contents().at(1)->contentType()->mimeType().data(), "application/pgp-signature");
    QCOMPARE(encapsulated->contents().first()->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void UnencryptedMessageTest::testSMIMESignedEncrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("smime-signed-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(true);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), MimeTreeParser::KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "multipart/signed");
    QCOMPARE(unencryptedMessage->contents().size(), 2);
    QCOMPARE(unencryptedMessage->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().at(1)->contentType()->mimeType().data(), "application/pkcs7-signature");
    QCOMPARE(unencryptedMessage->contents().first()->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void UnencryptedMessageTest::testOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("openpgp-signed-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(true);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), MimeTreeParser::KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "multipart/signed");
    QCOMPARE(unencryptedMessage->contents().size(), 2);
    QCOMPARE(unencryptedMessage->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().at(1)->contentType()->mimeType().data(), "application/pgp-signature");
    QCOMPARE(unencryptedMessage->contents().first()->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void UnencryptedMessageTest::testOpenPGPEncryptedAndSigned()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("openpgp-encrypted+signed.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(true);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), MimeTreeParser::KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().size(), 0);
    QCOMPARE(unencryptedMessage->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void UnencryptedMessageTest::testOpenPGPEncrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("openpgp-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(true);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->decodedContent().data(), "encrypted message text");
    QCOMPARE(unencryptedMessage->contents().size(), 0);
}

void UnencryptedMessageTest::testOpenPGPEncryptedNotDecrypted()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("openpgp-encrypted.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(false);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), MimeTreeParser::KMMsgFullyEncrypted);
    QCOMPARE(otp.plainTextContent().toLatin1().data(), "");

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE((bool) unencryptedMessage, false);
}

void UnencryptedMessageTest::testAsync_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("output");

    QTest::newRow("openpgp-encrypt") << QStringLiteral("openpgp-encrypted.mbox") << QStringLiteral("encrypted message text");
    QTest::newRow("smime-opaque-sign") << QStringLiteral("smime-opaque-sign.mbox") << QStringLiteral("A simple signed only test.");
    QTest::newRow("smime-encrypt") << QStringLiteral("smime-encrypted.mbox") << QStringLiteral("The quick brown fox jumped over the lazy dog.");
    QTest::newRow("openpgp-inline-encrypt") << QStringLiteral("openpgp-inline-charset-encrypted.mbox") << QStringLiteral("asdasd asd asd asdf sadf sdaf sadf \u00F6\u00E4\u00FC");
}

void UnencryptedMessageTest::testAsync()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, output);

    KMime::Message::Ptr originalMessage = Test::readAndParseMail(mailFileName);
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    emptySource.setAllowDecryption(true);
    {
        QEventLoop loop;
        MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);

        connect(&nodeHelper, &MimeTreeParser::NodeHelper::update, &loop, &QEventLoop::quit);
        otp.setAllowAsync(true);
        otp.parseObjectTree(originalMessage.data());
        loop.exec();
    }
    // Job ended
    {
        MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
        otp.setAllowAsync(true);
        otp.parseObjectTree(originalMessage.data());
        QCOMPARE(otp.plainTextContent(), output);
    }
}

void UnencryptedMessageTest::testNotDecrypted_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<bool>("decryptMessage");

    QTest::newRow("openpgp-inline") << QStringLiteral("inlinepgpencrypted.mbox") << true;
    QTest::newRow("openpgp-encrypt") << QStringLiteral("openpgp-encrypted.mbox") << true;
    QTest::newRow("smime-encrypt") << QStringLiteral("smime-encrypted.mbox") << true;
    QTest::newRow("openpgp-inline-encrypt") << QStringLiteral("openpgp-inline-charset-encrypted.mbox") << true;
    QTest::newRow("smime-opaque-sign") << QStringLiteral("smime-opaque-sign.mbox") << false;
    QTest::newRow("openpgp-inline-signed") << QStringLiteral("openpgp-inline-signed.mbox") << false;
    QTest::newRow("openpgp-mime-signed") << QStringLiteral("openpgp-signed-mailinglist.mbox") << false;
}

void UnencryptedMessageTest::testNotDecrypted()
{
    QFETCH(QString, mailFileName);
    QFETCH(bool, decryptMessage);
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(mailFileName);

    MimeTreeParser::NodeHelper nodeHelper;
    Test::HtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    emptySource.setAllowDecryption(false);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    if (decryptMessage) {
        QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    } else {
        QVERIFY(otp.plainTextContent().toLatin1().data());
    }
    QCOMPARE(testWriter.html.contains(QStringLiteral("<a href=\"kmail:decryptMessage\">")), decryptMessage);

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE((bool) unencryptedMessage, false);
}

void UnencryptedMessageTest::testSMimeAutoCertImport()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("smime-cert.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    Test::HtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    QVERIFY(testWriter.html.contains(QStringLiteral("Sorry, certificate could not be imported.")));
}