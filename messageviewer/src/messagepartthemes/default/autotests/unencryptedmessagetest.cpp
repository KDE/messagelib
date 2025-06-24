/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "unencryptedmessagetest.h"
using namespace Qt::Literals::StringLiterals;

#include "setupenv.h"
#include "util.h"

#include <MessageViewer/BufferedHtmlWriter>
#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>

#include <QTest>

using namespace MessageViewer;

QTEST_MAIN(UnencryptedMessageTest)

void UnencryptedMessageTest::initTestCase()
{
    Test::setupEnv();
}

void UnencryptedMessageTest::testNotDecrypted_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<bool>("decryptMessage");

    QTest::newRow("openpgp-inline") << u"inlinepgpencrypted.mbox"_s << true;
    QTest::newRow("openpgp-encrypt") << u"openpgp-encrypted.mbox"_s << true;
    QTest::newRow("smime-encrypt") << u"smime-encrypted.mbox"_s << true;
    QTest::newRow("openpgp-inline-encrypt") << u"openpgp-inline-charset-encrypted.mbox"_s << true;
    QTest::newRow("smime-opaque-sign") << u"smime-opaque-sign.mbox"_s << false;
    QTest::newRow("openpgp-inline-signed") << u"openpgp-inline-signed.mbox"_s << false;
    QTest::newRow("openpgp-mime-signed") << u"openpgp-signed-mailinglist.mbox"_s << false;
}

void UnencryptedMessageTest::testNotDecrypted()
{
    QFETCH(QString, mailFileName);
    QFETCH(bool, decryptMessage);
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(mailFileName);

    MimeTreeParser::NodeHelper nodeHelper;
    BufferedHtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    emptySource.setAllowDecryption(false);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    testWriter.begin();
    emptySource.render(otp.parsedPart(), false);
    testWriter.end();

    if (decryptMessage) {
        QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    } else {
        QVERIFY(otp.plainTextContent().toLatin1().data());
    }
    QCOMPARE(testWriter.data().contains("<a href=\"kmail:decryptMessage\""), decryptMessage);

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE((bool)unencryptedMessage, false);
    originalMessage.clear();
    unencryptedMessage.clear();
}

void UnencryptedMessageTest::testSMimeAutoCertImport()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(u"smime-cert.mbox"_s);

    MimeTreeParser::NodeHelper nodeHelper;
    BufferedHtmlWriter testWriter;
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());

    testWriter.begin();
    emptySource.render(otp.parsedPart(), false);
    testWriter.end();

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    QVERIFY(testWriter.data().contains("Sorry, certificate could not be imported."));
    originalMessage.clear();
}

#include "moc_unencryptedmessagetest.cpp"
