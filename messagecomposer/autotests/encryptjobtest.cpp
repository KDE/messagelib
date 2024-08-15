/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "encryptjobtest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
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

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>

#include <QGpgME/DecryptVerifyJob>
#include <QGpgME/Protocol>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/verificationresult.h>

#include <cstdlib>

#include <QTest>
#include <decryptionresult.h>

QTEST_MAIN(EncryptJobTest)

using namespace MessageComposer;

void EncryptJobTest::initTestCase()
{
    Test::setupEnv();
}

void EncryptJobTest::testContentDirect()
{
    Composer composer;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(QStringLiteral("one flew over the cuckoo's nest"));

    auto mainTextJob = new MainTextJob(&part, &composer);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    auto eJob = new EncryptJob(&composer);

    QVERIFY(eJob);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    eJob->setContent(mainTextJob->content());
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    checkEncryption(eJob);
}

void EncryptJobTest::testContentChained()
{
    Composer composer;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(QStringLiteral("one flew over the cuckoo's nest"));

    auto mainTextJob = new MainTextJob(&part, &composer);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const std::vector<GpgME::Key> &keys = Test::getKeys();
    auto eJob = new EncryptJob(&composer);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setContent(mainTextJob->content());

    checkEncryption(eJob);
}

void EncryptJobTest::testContentSubjobChained()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QByteArray data(QStringLiteral("one flew over the cuckoo's nest").toUtf8());
    KMime::Message skeletonMessage;

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    Composer composer;
    auto eJob = new EncryptJob(&composer);

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setSkeletonMessage(&skeletonMessage);
    eJob->appendSubjob(tJob);

    checkEncryption(eJob);
}

void EncryptJobTest::testHeaders()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    auto eJob = new EncryptJob(this);

    QVERIFY(eJob);

    const QByteArray data(QStringLiteral("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    eJob->setContent(content);
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    VERIFYEXEC(eJob);

    QByteArray mimeType("multipart/encrypted");

    KMime::Content *result = eJob->content();
    result->assemble();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType(false)->mimeType(), mimeType);
    QCOMPARE(result->contentType(false)->charset(), "UTF-8");
    QCOMPARE(result->contentType(false)->parameter("protocol"), QStringLiteral("application/pgp-encrypted"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
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
    QFETCH(bool, protectedHeaders);
    QFETCH(bool, protectedHeadersObvoscate);
    QFETCH(QString, referenceFile);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;
    auto eJob = new EncryptJob(&composer);

    QVERIFY(eJob);

    const QByteArray data(QStringLiteral("one flew over the cuckoo's nest").toUtf8());
    const QString subject(QStringLiteral("asdfghjklö"));

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.subject(true)->fromUnicodeString(subject);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

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

    KMime::Content *encPart = Util::findTypeInMessage(result, "application", "octet-stream");
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

    Test::compareFile(&tempNode, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void EncryptJobTest::testSetGnupgHome()
{
    Composer composer;

    KMime::Content content;
    content.setBody("one flew over the cuckoo's nest");

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    QTemporaryDir dir;
    {
        auto eJob = new EncryptJob(&composer);
        QVERIFY(eJob);

        eJob->setContent(&content);
        eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
        eJob->setRecipients(recipients);
        eJob->setEncryptionKeys(keys);
        eJob->setGnupgHome(dir.path());
        QCOMPARE(eJob->exec(), false);
    }

    for (const auto &key : keys) {
        Test::populateKeyring(dir.path(), key);
    }
    auto eJob = new EncryptJob(&composer);
    QVERIFY(eJob);

    eJob->setContent(&content);
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setGnupgHome(dir.path());
    checkEncryption(eJob);
}

void EncryptJobTest::checkEncryption(EncryptJob *eJob)
{
    VERIFYEXEC(eJob);

    KMime::Content *result = eJob->content();
    Q_ASSERT(result);
    result->assemble();

    ComposerTestUtil::verifyEncryption(result, QStringLiteral("one flew over the cuckoo's nest").toUtf8(), Kleo::OpenPGPMIMEFormat);

    delete result;
}

#include "moc_encryptjobtest.cpp"
