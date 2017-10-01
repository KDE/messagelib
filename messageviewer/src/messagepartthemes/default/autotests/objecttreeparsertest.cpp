/* Copyright 2009 Thomas McGuire <mcguire@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "objecttreeparsertest.h"
#include "util.h"
#include "setupenv.h"

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/BufferedHtmlWriter>
#include <MessageViewer/CSSHelperBase>

#include <QTest>

using namespace MessageViewer;
using namespace MimeTreeParser;

QTEST_MAIN(ObjectTreeParserTester)

void ObjectTreeParserTester::initTestCase()
{
    Test::setupEnv();
}

void ObjectTreeParserTester::test_parsePlainMessage()
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
    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    ObjectTreeParser otp(&emptySource);
    otp.parseObjectTree(msg.data());

    // Check that the textual content and the charset have the expected values
    QCOMPARE(otp.plainTextContent(), QStringLiteral("This is the message text.\n"));
    QVERIFY(otp.htmlContent().isEmpty());
    QCOMPARE(otp.plainTextContentCharset().toLower(), QByteArray("iso-8859-15"));

    // Check that the message was not modified in any way
    QCOMPARE(msg->encodedContent().constData(), content.constData());

    // Test that the charset of messages without an explicit charset declaration
    // is correct
    content
        = "From: Thomas McGuire <dontspamme@gmx.net>\n"
          "Subject: Plain Message Test\n"
          "Date: Wed, 5 Aug 2009 10:58:27 +0200\n"
          "MIME-Version: 1.0\n"
          "Content-Type: text/plain;\n"
          "\n"
          "This is the message text.\n";
    msg->setContent(content);
    msg->parse();
    ObjectTreeParser otp2(&emptySource);
    otp2.parseObjectTree(msg.data());
    QCOMPARE(otp2.plainTextContentCharset().constData(), msg->defaultCharset().constData());
}

void ObjectTreeParserTester::test_parseEncapsulatedMessage()
{
    KMime::Message::Ptr msg
        = Test::readAndParseMail(QStringLiteral("encapsulated-with-attachment.mbox"));
    QCOMPARE(msg->subject()->as7BitString(false).constData(), "Fwd: Test with attachment");
    QCOMPARE(msg->contents().size(), 2);

    // Parse the message
    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    // Check that the OTP didn't modify the message in weird ways
    QCOMPARE(msg->contents().size(), 2);
    QCOMPARE(msg->contents().at(0)->contents().size(), 0);
    QCOMPARE(msg->contents().at(1)->contents().size(), 1);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().size(), 2);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().at(0)->contents().size(), 0);
    QCOMPARE(msg->contents().at(1)->contents().first()->contents().at(1)->contents().size(), 0);

    // Check that the textual content and the charset have the expected values
    QCOMPARE(otp.plainTextContent(), QStringLiteral("This is the first encapsulating message.\n"));
    QCOMPARE(otp.plainTextContentCharset().toLower(), QByteArray("iso-8859-15"));
    QVERIFY(otp.htmlContent().isEmpty());

    // Check that the objecttreeparser did process the encapsulated message
    KMime::Message::Ptr encapsulated = msg->contents().at(1)->bodyAsMessage();
    QVERIFY(encapsulated.data());
    QVERIFY(nodeHelper.nodeProcessed(encapsulated.data()));
    QVERIFY(nodeHelper.nodeProcessed(encapsulated->contents().at(0)));
    QVERIFY(nodeHelper.nodeProcessed(encapsulated->contents().at(1)));
    QVERIFY(nodeHelper.partMetaData(msg->contents().at(1)).isEncapsulatedRfc822Message);
}

void ObjectTreeParserTester::test_missingContentTypeHeader()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("no-content-type.mbox"));
    QCOMPARE(msg->subject()->as7BitString(
                 false).constData(), "Simple Mail Without Content-Type Header");
    QCOMPARE(msg->contents().size(), 0);

    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "asdfasdf");
    QVERIFY(otp.htmlContent().isEmpty());
}

void ObjectTreeParserTester::test_inlinePGPDecryption()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("inlinepgpencrypted.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "inlinepgpencrypted");
    QCOMPARE(msg->contents().size(), 0);

    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource, &nodeHelper);

    emptySource.setAllowDecryption(true);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "some random text");

    // This test is only a workaround, till we can set the memento to the propper node of the mail.
    KMime::Content *content = new KMime::Content;
    QVERIFY(nodeHelper.bodyPartMemento(content, "decryptverify"));

    QVERIFY(otp.htmlContent().isEmpty());
}

void ObjectTreeParserTester::test_inlinePGPSigned()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("openpgp-inline-signed.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "test");
    QCOMPARE(msg->contents().size(), 0);

    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    NodeHelper nodeHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource, &nodeHelper);

    emptySource.setAllowDecryption(true);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    // This test is only a workaround, till we can set the memento to the propper node of the mail.
    QVERIFY(nodeHelper.bodyPartMemento(nullptr, "verification"));
}

void ObjectTreeParserTester::test_HTML()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("html.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 2);

    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    ObjectTreeParser otp(&emptySource);

    otp.parseObjectTree(msg.data());

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "Some HTML text");
    QVERIFY(otp.htmlContent().contains(QStringLiteral(
                                           "Some <span style=\" font-weight:600;\">HTML</span> text")));
    QCOMPARE(otp.htmlContentCharset().data(), "windows-1252");
}

void ObjectTreeParserTester::test_HTMLasText()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("html.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 2);

    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource);
    emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    QCOMPARE(otp.htmlContent().toLatin1().constData(), "");
    QCOMPARE(otp.htmlContentCharset().constData(), "");
    QCOMPARE(otp.plainTextContent().toLatin1().constData(), "Some HTML text");
    QCOMPARE(otp.plainTextContentCharset().constData(), "windows-1252");
}

void ObjectTreeParserTester::test_HTMLOnly()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("htmlonly.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    Test::ObjectTreeSource emptySource(nullptr, nullptr);
    ObjectTreeParser otp(&emptySource);

    otp.parseObjectTree(msg.data());

    QVERIFY(otp.plainTextContent().isEmpty());
    QVERIFY(otp.htmlContent().contains(QStringLiteral("<b>SOME</b> HTML text.")));
}

void ObjectTreeParserTester::test_HTMLOnlyText()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("htmlonly.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    MimeTreeParser::BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource);

    emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
    otp.parseObjectTree(msg.data());
    testWriter.end();

    QVERIFY(otp.plainTextContent().isEmpty());
    QVERIFY(otp.htmlContent().contains(QStringLiteral("<b>SOME</b> HTML text.")));
    QVERIFY(testWriter.data().contains("This is an HTML message. For security reasons, only the raw HTML code is shown."));
    QVERIFY(testWriter.data().contains("SOME* HTML text. <br>"));
}

void ObjectTreeParserTester::test_HTMLExternal()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("htmlonlyexternal.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().contains(QStringLiteral("<b>SOME</b> HTML text.")));
        QVERIFY(testWriter.data().contains("<b>SOME</b> HTML text."));
        QVERIFY(testWriter.data().contains("This HTML message may contain external references to images etc. For security/privacy reasons external references are not loaded."));
    }
    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setHtmlLoadExternal(true);
        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.htmlContent().contains(QStringLiteral("<b>SOME</b> HTML text.")));
        QVERIFY(testWriter.data().contains("<b>SOME</b> HTML text."));
        QVERIFY(!testWriter.data().contains("This HTML message may contain external references to images etc. For security/privacy reasons external references are not loaded."));
    }
}

void ObjectTreeParserTester::test_Alternative()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("alternative.mbox"));
    QCOMPARE(msg->contents().size(), 2);
    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.htmlContent().isEmpty());
        QVERIFY(otp.plainTextContent().contains(QStringLiteral(
                                                    "If you can see this text it means that your email client couldn't display our newsletter properly.")));
        QVERIFY(testWriter.data().contains("If you can see this text it means that your email client couldn't display our newsletter properly."));
    }

    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartHtml);
        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.plainTextContent().contains(QStringLiteral(
                                                    "If you can see this text it means that your email client couldn't display our newsletter properly.")));
        QVERIFY(otp.htmlContent().contains(QStringLiteral(
                                               "Some <span style=\" font-weight:600;\">HTML</span> text</p>")));
        QVERIFY(testWriter.data().contains("Some <span style=\" font-weight:600;\">HTML</span> text</p>"));
    }

    msg = Test::readAndParseMail(QStringLiteral("alternative-notext.mbox"));
    QCOMPARE(msg->contents().size(), 1);
    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().isEmpty());
        QVERIFY(testWriter.data().contains("Some *HTML* text"));
    }

    {
        MimeTreeParser::BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartHtml);
        otp.parseObjectTree(msg.data());
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().contains(QStringLiteral(
                                               "Some <span style=\" font-weight:600;\">HTML</span> text</p>")));
        QVERIFY(testWriter.data().contains("Some <span style=\" font-weight:600;\">HTML</span> text</p>"));
    }
}
