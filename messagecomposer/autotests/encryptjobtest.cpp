/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "encryptjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/ComposerJob>
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
#include <gpgme++/decryptionresult.h>

QTEST_MAIN(EncryptJobTest)

using namespace MessageComposer;

void EncryptJobTest::initTestCase()
{
    Test::setupEnv();
}

void EncryptJobTest::testContentDirect()
{
    ComposerJob composerJob;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(u"one flew over the cuckoo's nest"_s);

    auto mainTextJob = new MainTextJob(&part, &composerJob);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    auto eJob = new EncryptJob(&composerJob);

    QVERIFY(eJob);

    const QStringList recipients = {u"test@kolab.org"_s};

    eJob->setContent(mainTextJob->content());
    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    checkEncryption(eJob);
}

void EncryptJobTest::testContentChained()
{
    ComposerJob composerJob;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(u"one flew over the cuckoo's nest"_s);

    auto mainTextJob = new MainTextJob(&part, &composerJob);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const std::vector<GpgME::Key> &keys = Test::getKeys();
    auto eJob = new EncryptJob(&composerJob);

    const QStringList recipients = {u"test@kolab.org"_s};

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);
    eJob->setContent(mainTextJob->content());

    checkEncryption(eJob);
}

void EncryptJobTest::testContentSubjobChained()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QByteArray data(u"one flew over the cuckoo's nest"_s.toUtf8());
    KMime::Message skeletonMessage;

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    const QStringList recipients = {u"test@kolab.org"_s};

    ComposerJob composerJob;
    auto eJob = new EncryptJob(&composerJob);

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

    const QByteArray data(u"one flew over the cuckoo's nest"_s.toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    const QStringList recipients = {u"test@kolab.org"_s};

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
    QCOMPARE(result->contentType(false)->parameter("protocol"), u"application/pgp-encrypted"_s);
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}

void EncryptJobTest::testProtectedHeaders_data()
{
    QTest::addColumn<bool>("protectedHeaders");
    QTest::addColumn<bool>("protectedHeadersObvoscate");
    QTest::addColumn<QString>("referenceFile");

    QTest::newRow("simple-obvoscate") << true << true << u"protected_headers-obvoscate.mbox"_s;
    QTest::newRow("simple-non-obvoscate") << true << false << u"protected_headers-non-obvoscate.mbox"_s;
    QTest::newRow("non-protected_headers") << false << false << u"non-protected_headers.mbox"_s;
}

void EncryptJobTest::testProtectedHeaders()
{
    QFETCH(bool, protectedHeaders);
    QFETCH(bool, protectedHeadersObvoscate);
    QFETCH(QString, referenceFile);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    ComposerJob composerJob;
    auto eJob = new EncryptJob(&composerJob);

    QVERIFY(eJob);

    const QByteArray data(u"one flew over the cuckoo's nest"_s.toUtf8());
    const QString subject(u"asdfghjklö"_s);

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    KMime::Message skeletonMessage;
    skeletonMessage.contentType(true)->setMimeType("foo/bla");
    skeletonMessage.to(true)->from7BitString("to@test.de, to2@test.de");
    skeletonMessage.cc(true)->from7BitString("cc@test.de, cc2@test.de");
    skeletonMessage.bcc(true)->from7BitString("bcc@test.de, bcc2@test.de");
    skeletonMessage.subject(true)->fromUnicodeString(subject);

    const QStringList recipients = {u"test@kolab.org"_s};

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
    ComposerJob composerJob;

    KMime::Content content;
    content.setBody("one flew over the cuckoo's nest");

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QStringList recipients = {u"test@kolab.org"_s};

    QTemporaryDir dir;
    {
        auto eJob = new EncryptJob(&composerJob);
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
    auto eJob = new EncryptJob(&composerJob);
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

    ComposerTestUtil::verifyEncryption(result, u"one flew over the cuckoo's nest"_s.toUtf8(), Kleo::OpenPGPMIMEFormat);

    delete result;
}

#include "moc_encryptjobtest.cpp"
