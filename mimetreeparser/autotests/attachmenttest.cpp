/*
  SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "objecttreeparser.h"
#include "util.h"

#include "setupenv.h"
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QTest>

using namespace MimeTreeParser;

class AttachmentTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void testEncryptedAttachment_data();
    void testEncryptedAttachment();
};

QTEST_MAIN(AttachmentTest)

void AttachmentTest::initTestCase()
{
    MimeTreeParser::Test::setupEnv();
}

void AttachmentTest::testEncryptedAttachment_data()
{
    QTest::addColumn<QString>("mbox");
    QTest::newRow("encrypted") << "openpgp-encrypted-two-attachments.mbox";
    QTest::newRow("signed") << "openpgp-signed-two-attachments.mbox";
    QTest::newRow("signed+encrypted") << "openpgp-signed-encrypted-two-attachments.mbox";
    QTest::newRow("encrypted+partial signed") << "openpgp-encrypted-partially-signed-attachments.mbox";
}

void AttachmentTest::testEncryptedAttachment()
{
    QFETCH(QString, mbox);
    auto msg = readAndParseMail(mbox);
    NodeHelper nodeHelper;
    SimpleObjectTreeSource testSource;
    testSource.setDecryptMessage(true);
    ObjectTreeParser otp(&testSource, &nodeHelper);
    otp.parseObjectTree(msg.data());

    auto attachments = msg->attachments();
    auto encAtts = nodeHelper.attachmentsOfExtraContents();
    QCOMPARE(attachments.size() + encAtts.size(), 2);
    msg.clear();
}

#include "attachmenttest.moc"
