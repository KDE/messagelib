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
#include <MessageViewer/BufferedHtmlWriter>

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
    QTest::newRow("openpgp-inline-encrypt") << QStringLiteral(
        "openpgp-inline-charset-encrypted.mbox") << true;
    QTest::newRow("smime-opaque-sign") << QStringLiteral("smime-opaque-sign.mbox") << false;
    QTest::newRow("openpgp-inline-signed") << QStringLiteral("openpgp-inline-signed.mbox") << false;
    QTest::newRow("openpgp-mime-signed") << QStringLiteral("openpgp-signed-mailinglist.mbox")
                                         << false;
}

void UnencryptedMessageTest::testNotDecrypted()
{
    QFETCH(QString, mailFileName);
    QFETCH(bool, decryptMessage);
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(mailFileName);

    MimeTreeParser::NodeHelper nodeHelper;
    BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    emptySource.setAllowDecryption(false);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());
    testWriter.end();

    if (decryptMessage) {
        QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    } else {
        QVERIFY(otp.plainTextContent().toLatin1().data());
    }
    QCOMPARE(testWriter.data().contains("<a href=\"kmail:decryptMessage\">"), decryptMessage);

    KMime::Message::Ptr unencryptedMessage = nodeHelper.unencryptedMessage(originalMessage);
    QCOMPARE((bool)unencryptedMessage, false);
}

void UnencryptedMessageTest::testSMimeAutoCertImport()
{
    KMime::Message::Ptr originalMessage = Test::readAndParseMail(QStringLiteral("smime-cert.mbox"));

    MimeTreeParser::NodeHelper nodeHelper;
    BufferedHtmlWriter testWriter;
    testWriter.begin();
    Test::CSSHelper testCSSHelper;
    Test::ObjectTreeSource emptySource(&testWriter, &testCSSHelper);
    MimeTreeParser::ObjectTreeParser otp(&emptySource, &nodeHelper);
    otp.parseObjectTree(originalMessage.data());
    testWriter.end();

    QCOMPARE(otp.plainTextContent().toLatin1().data(), "");
    QVERIFY(testWriter.data().contains("Sorry, certificate could not be imported."));
}
