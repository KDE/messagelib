/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  SPDX-FileCopyrightText: 2019 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "basicobjecttreeparsertest.h"
#include "setupenv.h"
#include "util.h"

#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QProcess>
#include <QTest>

using namespace MimeTreeParser;

QTEST_MAIN(ObjectTreeParserTest)

void ObjectTreeParserTest::initTestCase()
{
    Test::setupEnv();
    qputenv("TZ", "GMT");
}

QString stringifyMessagePartTree(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    const QString line = QStringLiteral("%1 * %3\n").arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    QString ret = line;

    indent += QLatin1Char(' ');
    const auto subParts = messagePart->subParts();
    for (const auto &subPart : subParts) {
        ret += stringifyMessagePartTree(subPart, indent);
    }
    return ret;
}

void testMessagePartTree(const MessagePart::Ptr &messagePart, const QString &mailFileName)
{
    QString renderedTree = stringifyMessagePartTree(messagePart, QString());

    const QString treeFileName = QLatin1StringView(MAIL_DATA_DIR) + QLatin1Char('/') + mailFileName + QStringLiteral(".tree");
    const QString outTreeFileName = mailFileName + QStringLiteral(".tree");

    {
        QFile f(outTreeFileName);
        f.open(QIODevice::WriteOnly);
        f.write(renderedTree.toUtf8());
        f.close();
    }
    // compare to reference file
    const QStringList args = QStringList() << QStringLiteral("-u") << treeFileName << outTreeFileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QStringLiteral("diff"), args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}

void ObjectTreeParserTest::testMailWithoutEncryption()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox"));
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());
    QVERIFY(!nodeHelper.unencryptedMessage(originalMessage));
}

void ObjectTreeParserTest::testBinaryAttachmentNotPGP()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("binary-attachment-not-pgp.mbox"));
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(originalMessage->contentType()->mimeType().data(), "multipart/mixed");
    QCOMPARE(originalMessage->contents().size(), 2);
    QCOMPARE(originalMessage->contents().at(0)->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(originalMessage->contents().at(1)->contentType()->mimeType().data(), "application/msword");

    QCOMPARE(nodeHelper.encryptionState(originalMessage->contents().at(0)), KMMsgNotEncrypted);
    QCOMPARE(nodeHelper.encryptionState(originalMessage->contents().at(1)), KMMsgNotEncrypted);
}

void ObjectTreeParserTest::testSignedForwardedOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("signed-forward-openpgp-signed-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "bla bla bla"); // The textual content doesn't include the encrypted encapsulated message by design
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgPartiallyEncrypted);
    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), KMMsgFullySigned);

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QVERIFY(!unencryptedMessage); // We must not invalidate the outer signature
}

void ObjectTreeParserTest::testForwardedOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("forward-openpgp-signed-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "bla bla bla"); // The textual content doesn't include the encrypted encapsulated message by design
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgPartiallyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), KMMsgPartiallySigned);

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

void ObjectTreeParserTest::testSMIMESignedEncrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("smime-signed-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "multipart/signed");
    QCOMPARE(unencryptedMessage->contents().size(), 2);
    QCOMPARE(unencryptedMessage->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().at(1)->contentType()->mimeType().data(), "application/pkcs7-signature");
    QCOMPARE(unencryptedMessage->contents().first()->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void ObjectTreeParserTest::testOpenPGPSignedEncrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("openpgp-signed-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "multipart/signed");
    QCOMPARE(unencryptedMessage->contents().size(), 2);
    QCOMPARE(unencryptedMessage->contents().first()->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().at(1)->contentType()->mimeType().data(), "application/pgp-signature");
    QCOMPARE(unencryptedMessage->contents().first()->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void ObjectTreeParserTest::testOpenPGPEncryptedAndSigned()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("openpgp-encrypted+signed.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);

    QCOMPARE(nodeHelper.overallSignatureState(originalMessage.data()), KMMsgFullySigned);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->contents().size(), 0);
    QCOMPARE(unencryptedMessage->decodedContent().data(), "encrypted message text");

    // TODO: Check that the signature is valid
}

void ObjectTreeParserTest::testOpenPGPEncrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("openpgp-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "encrypted message text");
    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);

    // Now, test that the unencrypted message is generated correctly
    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE(unencryptedMessage->contentType()->mimeType().data(), "text/plain");
    QCOMPARE(unencryptedMessage->decodedContent().data(), "encrypted message text");
    QCOMPARE(unencryptedMessage->contents().size(), 0);
}

void ObjectTreeParserTest::testOpenPGPEncryptedNotDecrypted()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("openpgp-encrypted.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);
    QCOMPARE(otp.plainTextContent().toLatin1().data(), "");

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QVERIFY(!unencryptedMessage);
}

void ObjectTreeParserTest::testOpenPGPEncryptedOverrideEncoding()
{
    KMime::Message::Ptr originalMessage = readAndParseMail(QStringLiteral("openpgp-encrypted-attachment-iso-8859-15-encoded.mbox"));

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    const QString data = QStringLiteral(
        "german umlauts ISO-8859-15:\n\u00c3\u20ac - a with two dots - utf-8: U+00C3 U20AC\n\u00c3\u009f - eszett or \"sharp s\" - utf-8: U+00C3 U+009F\n");
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    QCOMPARE(nodeHelper.overallEncryptionState(originalMessage.data()), KMMsgFullyEncrypted);

    QVERIFY(otp.plainTextContent() != data); // it is not utf-8

    testSource.setOverrideCodecName("iso-8859-15");
    otp.parseObjectTree(originalMessage.data());
    QCOMPARE(otp.plainTextContent(), data);
}

void ObjectTreeParserTest::testAsync_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("output");

    QTest::newRow("openpgp-encrypt") << QStringLiteral("openpgp-encrypted.mbox") << QStringLiteral("encrypted message text");
    QTest::newRow("smime-opaque-sign") << QStringLiteral("smime-opaque-sign.mbox") << QStringLiteral("A simple signed only test.");
    QTest::newRow("smime-encrypt") << QStringLiteral("smime-encrypted.mbox") << QStringLiteral("The quick brown fox jumped over the lazy dog.");
    QTest::newRow("openpgp-inline-encrypt") << QStringLiteral("openpgp-inline-charset-encrypted.mbox")
                                            << QStringLiteral("asdasd asd asd asdf sadf sdaf sadf \u00F6\u00E4\u00FC");
}

void ObjectTreeParserTest::testAsync()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, output);

    KMime::Message::Ptr originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    {
        QEventLoop loop;
        ObjectTreeParser otp(&testSource, &nodeHelper);

        connect(&nodeHelper, &NodeHelper::update, &loop, &QEventLoop::quit);
        otp.setAllowAsync(true);
        otp.parseObjectTree(originalMessage.data());
        loop.exec();
    }
    // Job ended
    {
        ObjectTreeParser otp(&testSource, &nodeHelper);
        otp.setAllowAsync(true);
        otp.parseObjectTree(originalMessage.data());
        QCOMPARE(otp.plainTextContent(), output);
    }
}

void ObjectTreeParserTest::testHtmlContent_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<QString>("output");

    QTest::newRow("html-attachments1") << QStringLiteral("html-attachment1.mbox")
                                       << QStringLiteral("<html><head></head><body><p><span style=\"font-family:Arial;\">A Body Text</span></p></body></html>");
    QTest::newRow("html-attachments2") << QStringLiteral("html-attachment2.mbox") << QStringLiteral("<html><head></head><body>HTML Text</body></html>");
}

void ObjectTreeParserTest::testHtmlContent()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, output);

    KMime::Message::Ptr originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    QVERIFY(otp.plainTextContent().isEmpty());
    QCOMPARE(otp.htmlContent(), output);
}

void ObjectTreeParserTest::testRenderedTree_data()
{
    QTest::addColumn<QString>("mailFileName");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const auto l = dir.entryList(QStringList(QStringLiteral("*.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : l) {
        if (!QFile::exists(dir.path() + QLatin1Char('/') + file + QStringLiteral(".tree"))) {
            continue;
        }
        QTest::newRow(file.toLatin1().constData()) << file;
    }
}

void ObjectTreeParserTest::testRenderedTree()
{
    QFETCH(QString, mailFileName);
    KMime::Message::Ptr originalMessage = readAndParseMail(mailFileName);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(originalMessage.data());

    testMessagePartTree(otp.parsedPart(), mailFileName);
}

void ObjectTreeParserTest::testParsePlainMessage()
{
    KMime::Message::Ptr msg(new KMime::Message());
    QByteArray content(
        "From: Thomas McGuire <dontspamme@gmx.net>\n"
        "Subject: Plain Message Test\n"
        "Date: Wed, 5 Aug 2009 10:58:27 +0200\n"
        "MIME-Version: 1.0\n"
        "Content-Type: text/plain;\n"
        "  charset=\"iso-8859-15\"\n"
        "\n"
        "This is the message text.\n");
    msg->setContent(content);
    msg->parse();

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "Plain Message Test");
    QCOMPARE(msg->contents().size(), 0);

    // Parse the message
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource);
    otp.parseObjectTree(msg.data());

    // Check that the textual content and the charset have the expected values
    QCOMPARE(otp.plainTextContent(), QStringLiteral("This is the message text.\n"));
    QVERIFY(otp.htmlContent().isEmpty());

    // Check that the message was not modified in any way
    QCOMPARE(msg->encodedContent().constData(), content.constData());

    // Test that the charset of messages without an explicit charset declaration
    // is correct
    content =
        "From: Thomas McGuire <dontspamme@gmx.net>\n"
        "Subject: Plain Message Test\n"
        "Date: Wed, 5 Aug 2009 10:58:27 +0200\n"
        "MIME-Version: 1.0\n"
        "Content-Type: text/plain;\n"
        "\n"
        "This is the message text.\n";
    msg->setContent(content);
    msg->parse();
    ObjectTreeParser otp2(&testSource);
    otp2.parseObjectTree(msg.data());
}

void ObjectTreeParserTest::testParseEncapsulatedMessage()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox"));
    QCOMPARE(msg->subject()->as7BitString(false).constData(), "Fwd: Test with attachment");
    QCOMPARE(msg->contents().size(), 2);

    // Parse the message
    SimpleObjectTreeSource testSource;
    NodeHelper nodeHelper;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(msg.data());

    // Check that the OTP didn't modify the message in weird ways
    QCOMPARE(msg->contents().size(), 2);
    QCOMPARE(msg->contents().at(0)->contents().size(), 0);
    QCOMPARE(msg->contents().at(1)->contents().size(), 1);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().size(), 2);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().at(0)->contents().size(), 0);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().at(1)->contents().size(), 0);

    // Check that the textual content and the charset have the expected values
    QCOMPARE(otp.plainTextContent(), QStringLiteral("This is the first encapsulating message.\n"));
    QVERIFY(otp.htmlContent().isEmpty());

    // Check that the objecttreeparser did process the encapsulated message
    KMime::Message::Ptr encapsulated = msg->contents().at(1)->bodyAsMessage();
    QVERIFY(encapsulated.data());
    QVERIFY(nodeHelper.nodeProcessed(encapsulated.data()));
    QVERIFY(nodeHelper.nodeProcessed(encapsulated->contents().at(0)));
    QVERIFY(nodeHelper.nodeProcessed(encapsulated->contents().at(1)));
    QVERIFY(nodeHelper.partMetaData(msg->contents().at(1)).isEncapsulatedRfc822Message);
}

void ObjectTreeParserTest::testMissingContentTypeHeader()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("no-content-type.mbox"));
    QCOMPARE(msg->subject()->as7BitString(false).constData(), "Simple Mail Without Content-Type Header");
    QCOMPARE(msg->contents().size(), 0);

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(msg.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "asdfasdf");
    QVERIFY(otp.htmlContent().isEmpty());
}

void ObjectTreeParserTest::testInlinePGPDecryption()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("inlinepgpencrypted.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "inlinepgpencrypted");
    QCOMPARE(msg->contents().size(), 0);

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);

    testSource.setDecryptMessage(true);
    otp.parseObjectTree(msg.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "some random text");

    // This test is only a workaround, till we can set the memento to the proper node of the mail.
    auto content = new KMime::Content;
    QVERIFY(nodeHelper.bodyPartMemento(content, "decryptverify-attachment:?place=0"));

    QVERIFY(otp.htmlContent().isEmpty());
}

void ObjectTreeParserTest::testInlinePGPSigned()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("openpgp-inline-signed.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "test");
    QCOMPARE(msg->contents().size(), 0);

    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource, &nodeHelper);

    testSource.setDecryptMessage(true);
    otp.parseObjectTree(msg.data());

    // This test is only a workaround, till we can set the memento to the proper node of the mail.
    QVERIFY(nodeHelper.bodyPartMemento(nullptr, "verification-attachment:?place=0"));
}

void ObjectTreeParserTest::testHTML()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("html.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 2);

    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource);

    otp.parseObjectTree(msg.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "Some HTML text");
    QVERIFY(otp.htmlContent().contains(QLatin1StringView("Some <span style=\" font-weight:600;\">HTML</span> text")));
}

void ObjectTreeParserTest::testHTMLasText()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("html.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 2);

    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource);

    testSource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
    otp.parseObjectTree(msg.data());

    QCOMPARE(otp.htmlContent().toLatin1().constData(), "");
    QCOMPARE(otp.plainTextContent().toLatin1().constData(), "Some HTML text");
}

void ObjectTreeParserTest::testHTMLOnly()
{
    KMime::Message::Ptr msg = readAndParseMail(QStringLiteral("htmlonly.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    SimpleObjectTreeSource testSource;
    ObjectTreeParser otp(&testSource);

    otp.parseObjectTree(msg.data());

    QVERIFY(otp.plainTextContent().isEmpty());
    QVERIFY(otp.htmlContent().contains(QLatin1StringView("<b>SOME</b> HTML text.")));
}

#include "moc_basicobjecttreeparsertest.cpp"
