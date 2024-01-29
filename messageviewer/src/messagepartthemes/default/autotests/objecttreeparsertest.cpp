/* SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#include "objecttreeparsertest.h"
#include "setupenv.h"
#include "util.h"

#include <MessageViewer/BufferedHtmlWriter>
#include <MimeTreeParser/ObjectTreeParser>

#include <QTest>

using namespace MessageViewer;
using namespace MimeTreeParser;

QTEST_MAIN(ObjectTreeParserTester)

void ObjectTreeParserTester::initTestCase()
{
    Test::setupEnv();
}

void ObjectTreeParserTester::test_HTMLOnlyText()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("htmlonly.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    BufferedHtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    ObjectTreeParser otp(&emptySource);

    emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
    otp.parseObjectTree(msg.data());

    testWriter.begin();
    emptySource.render(otp.parsedPart(), false);
    testWriter.end();

    QVERIFY(otp.plainTextContent().isEmpty());
    QVERIFY(otp.htmlContent().contains(QLatin1StringView("<b>SOME</b> HTML text.")));
    QVERIFY(testWriter.data().contains("This is an HTML message. For security reasons, only the raw HTML code is shown."));
    QVERIFY(testWriter.data().contains("SOME* HTML text. <br>"));
    msg.clear();
}

void ObjectTreeParserTester::test_HTMLExternal()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("htmlonlyexternal.mbox"));

    QCOMPARE(msg->subject()->as7BitString(false).constData(), "HTML test");
    QCOMPARE(msg->contents().size(), 0);

    {
        BufferedHtmlWriter testWriter;
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().contains(QLatin1StringView("<b>SOME</b> HTML text.")));
        QVERIFY(testWriter.data().contains("<b>SOME</b> HTML text."));
        QVERIFY(testWriter.data().contains(
            "This HTML message may contain external references to images etc. For security/privacy reasons external references are not loaded."));
    }
    {
        BufferedHtmlWriter testWriter;
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setHtmlLoadExternal(true);
        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.htmlContent().contains(QLatin1StringView("<b>SOME</b> HTML text.")));
        QVERIFY(testWriter.data().contains("<b>SOME</b> HTML text."));
        QVERIFY(!testWriter.data().contains(
            "This HTML message may contain external references to images etc. For security/privacy reasons external references are not loaded."));
    }
    msg.clear();
}

void ObjectTreeParserTester::test_Alternative()
{
    KMime::Message::Ptr msg = Test::readAndParseMail(QStringLiteral("alternative.mbox"));
    QCOMPARE(msg->contents().size(), 2);
    {
        BufferedHtmlWriter testWriter;
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.htmlContent().isEmpty());
        QVERIFY(otp.plainTextContent().contains(
            QLatin1StringView("If you can see this text it means that your email client couldn't display our newsletter properly.")));
        QVERIFY(testWriter.data().contains("If you can see this text it means that your email client couldn't display our newsletter properly."));
    }

    {
        BufferedHtmlWriter testWriter;
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartHtml);
        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.plainTextContent().contains(
            QLatin1StringView("If you can see this text it means that your email client couldn't display our newsletter properly.")));
        QVERIFY(otp.htmlContent().contains(QLatin1StringView("Some <span style=\" font-weight:600;\">HTML</span> text</p>")));
        QVERIFY(testWriter.data().contains("Some <span style=\" font-weight:600;\">HTML</span> text</p>"));
    }

    msg.clear();
    msg = Test::readAndParseMail(QStringLiteral("alternative-notext.mbox"));
    QCOMPARE(msg->contents().size(), 1);
    {
        BufferedHtmlWriter testWriter;
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartPlain);
        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().isEmpty());
        QVERIFY(testWriter.data().contains("Some *HTML* text"));
    }

    {
        BufferedHtmlWriter testWriter;
        testWriter.begin();
        Test::CSSHelper testCSSHelper;
        Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
        ObjectTreeParser otp(&emptySource);

        emptySource.setPreferredMode(MimeTreeParser::Util::MultipartHtml);
        otp.parseObjectTree(msg.data());

        testWriter.begin();
        emptySource.render(otp.parsedPart(), false);
        testWriter.end();

        QVERIFY(otp.plainTextContent().isEmpty());
        QVERIFY(otp.htmlContent().contains(QLatin1StringView("Some <span style=\" font-weight:600;\">HTML</span> text</p>")));
        QVERIFY(testWriter.data().contains("Some <span style=\" font-weight:600;\">HTML</span> text</p>"));
    }
    msg.clear();
}

#include "moc_objecttreeparsertest.cpp"
