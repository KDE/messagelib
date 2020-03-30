/*
  Copyright (C) 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Leo Franchi <lfranchi@kde.org>

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

#include "encryptjobtest.h"

#include "qtest_messagecomposer.h"
#include "cryptofunctions.h"
#include "setupenv.h"

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/EncryptJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/TextPart>
#include <MessageComposer/TransparentJob>
#include <MessageComposer/Util>

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/NodeHelper>

#include <QGpgME/Protocol>
#include <QGpgME/DecryptVerifyJob>

#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>

#include <stdlib.h>
#include <KCharsets>

#include <QDebug>
#include <QTest>
#include <decryptionresult.h>

QTEST_MAIN(EncryptJobTest)

using namespace MessageComposer;

void EncryptJobTest::initTestCase()
{
    MessageComposer::Test::setupEnv();
}

void EncryptJobTest::testContentDirect()
{
    MessageComposer::Composer *composer = new MessageComposer::Composer;
    QVector<QByteArray> charsets;
    charsets << "us-ascii";
    composer->globalPart()->setCharsets(charsets);
    MessageComposer::TextPart *part = new MessageComposer::TextPart(this);
    part->setWordWrappingEnabled(false);
    part->setCleanPlainText(QStringLiteral("one flew over the cuckoo's nest"));

    MessageComposer::MainTextJob *mainTextJob = new MessageComposer::MainTextJob(part, composer);

    QVERIFY(composer);
    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    std::vector< GpgME::Key > keys = MessageComposer::Test::getKeys();

    MessageComposer::EncryptJob *eJob = new MessageComposer::EncryptJob(composer);

    QVERIFY(eJob);

    QStringList recipients;
    recipients << QString::fromLocal8Bit("test@kolab.org");

    eJob->setContent(mainTextJob->content());
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    checkEncryption(eJob);
}

void EncryptJobTest::testContentChained()
{
    MessageComposer::Composer *composer = new MessageComposer::Composer;
    QVector<QByteArray> charsets;
    charsets << "us-ascii";
    composer->globalPart()->setCharsets(charsets);
    MessageComposer::TextPart *part = new MessageComposer::TextPart(this);
    part->setWordWrappingEnabled(false);
    part->setCleanPlainText(QStringLiteral("one flew over the cuckoo's nest"));

    MessageComposer::MainTextJob *mainTextJob = new MessageComposer::MainTextJob(part, composer);

    QVERIFY(composer);
    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    std::vector< GpgME::Key > keys = MessageComposer::Test::getKeys();
    MessageComposer::EncryptJob *eJob = new MessageComposer::EncryptJob(composer);

    QStringList recipients;
    recipients << QString::fromLocal8Bit("test@kolab.org");

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setContent(mainTextJob->content());

    checkEncryption(eJob);
}

void EncryptJobTest::testContentSubjobChained()
{
    std::vector< GpgME::Key > keys = MessageComposer::Test::getKeys();

    QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    KMime::Message skeletonMessage;

    KMime::Content *content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    QStringList recipients;
    recipients << QString::fromLocal8Bit("test@kolab.org");

    Composer composer;
    auto eJob = new MessageComposer::EncryptJob(&composer);

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setSkeletonMessage(&skeletonMessage);
    eJob->appendSubjob(tJob);

    checkEncryption(eJob);
}

void EncryptJobTest::testHeaders()
{
    std::vector< GpgME::Key > keys = MessageComposer::Test::getKeys();

    MessageComposer::EncryptJob *eJob = new MessageComposer::EncryptJob(this);

    QVERIFY(eJob);

    QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    KMime::Content *content = new KMime::Content;
    content->setBody(data);

    QStringList recipients;
    recipients << QString::fromLocal8Bit("test@kolab.org");

    eJob->setContent(content);
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    VERIFYEXEC(eJob);

    QByteArray mimeType("multipart/encrypted");
    QByteArray charset("ISO-8859-1");

    KMime::Content *result = eJob->content();
    result->assemble();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), mimeType);
    QCOMPARE(result->contentType()->charset(), charset);
    QCOMPARE(result->contentType()->parameter(QString::fromLocal8Bit("protocol")), QString::fromLocal8Bit("application/pgp-encrypted"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);
}

void EncryptJobTest::testProtectedHeaders_data()
{
    QTest::addColumn<bool>("protectedHeaders");
    QTest::addColumn<bool>("protectedHeadersObvoscate");
    QTest::addColumn<QString>("referenceFile");

    QTest::newRow("simple-obvoscate") << true << true << QStringLiteral("protected_headers-obvoscate.mbox");
    QTest::newRow("simple-non-obvoscate") << true << false << QStringLiteral("protected_headers-non-obvoscate.mbox");
    QTest::newRow("non-protected_headers") << false << false << QStringLiteral("non-protected_headers.mbox");
}

void EncryptJobTest::testProtectedHeaders()
{
    QFETCH(bool,protectedHeaders);
    QFETCH(bool, protectedHeadersObvoscate);
    QFETCH(QString, referenceFile);

    std::vector< GpgME::Key > keys = MessageComposer::Test::getKeys();

    MessageComposer::Composer composer;
    MessageComposer::EncryptJob *eJob = new MessageComposer::EncryptJob(&composer);

    QVERIFY(eJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    const QString subject(QStringLiteral("asdfghjklö"));

    KMime::Content *content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.subject(true)->fromUnicodeString(subject, "utf-8");

    QStringList recipients;
    recipients << QString::fromLocal8Bit("test@kolab.org");

    eJob->setContent(content);
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setSkeletonMessage(&skeletonMessage);
    eJob->setProtectedHeaders(protectedHeaders);
    eJob->setProtectedHeadersObvoscate(protectedHeadersObvoscate);

    VERIFYEXEC(eJob);

    if (protectedHeadersObvoscate) {
        QCOMPARE(skeletonMessage.subject()->as7BitString(false), "...");
    } else {
        QCOMPARE(skeletonMessage.subject()->asUnicodeString(), subject);
    }

    KMime::Content *result = eJob->content();
    result->assemble();

    KMime::Content *encPart = MessageComposer::Util::findTypeInMessage(result, "application", "octet-stream");
    KMime::Content tempNode;
    {
        QByteArray plainText;
        auto job = QGpgME::openpgp()->decryptVerifyJob();
        job->exec(encPart->encodedBody(), plainText);

        tempNode.setContent(KMime::CRLFtoLF(plainText.constData()));
        tempNode.parse();
    }
    if (protectedHeadersObvoscate) {
        tempNode.contentType(false)->setBoundary("123456789");
        tempNode.assemble();
    }

    delete result;

    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    const QByteArray encodedContent(tempNode.encodedContent());
    f.write(encodedContent);
    if (!encodedContent.endsWith('\n')) {
        f.write("\n");
    }
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/")+referenceFile);
}

void EncryptJobTest::checkEncryption(MessageComposer::EncryptJob *eJob)
{
    VERIFYEXEC(eJob);

    KMime::Content *result = eJob->content();
    Q_ASSERT(result);
    result->assemble();

    ComposerTestUtil::verifyEncryption(result, QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8(), Kleo::OpenPGPMIMEFormat);
}
