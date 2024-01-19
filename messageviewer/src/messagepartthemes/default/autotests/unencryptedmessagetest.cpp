/*
  SPDX-FileCopyrightText: 2010 Thomas McGuire <thomas.mcguire@kdab.com>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "unencryptedmessagetest.h"
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
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("smime-cert.mbox"));

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
