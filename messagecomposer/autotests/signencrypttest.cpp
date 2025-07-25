/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signencrypttest.h"
using namespace Qt::Literals::StringLiterals;

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include <QTest>

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/SignEncryptJob>
#include <MessageComposer/TextPart>
#include <MessageComposer/TransparentJob>
#include <MessageComposer/Util>

#include <QGpgME/DecryptVerifyJob>
#include <QGpgME/Protocol>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/verificationresult.h>

#include <sstream>

#include "setupenv.h"

QTEST_MAIN(SignEncryptTest)

using namespace MessageComposer;

void SignEncryptTest::initTestCase()
{
    Test::setupEnv();
}

void SignEncryptTest::testContent_data()
{
    QTest::addColumn<int>("cryptoMessageFormat");
    QTest::addColumn<QString>("error");

    QTest::newRow("OpenPGPMimeFormat") << (int)Kleo::OpenPGPMIMEFormat << QString();
    QTest::newRow("InlineOpenPGPFormat") << (int)Kleo::InlineOpenPGPFormat << QString();
    QTest::newRow("SMIMEFormat") << (int)Kleo::SMIMEFormat << u"Not implemented"_s;
    QTest::newRow("SMIMEOpaqueFormat") << (int)Kleo::SMIMEOpaqueFormat << u"Not implemented"_s;
}

void SignEncryptTest::testContent()
{
    QFETCH(int, cryptoMessageFormat);
    QFETCH(QString, error);

    const std::vector<GpgME::Key> &keys = Test::getKeys();
    const QString data(QString::fromLocal8Bit("one flew over the cuckoo's nest"));

    ComposerJob composerJob;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(data);

    auto mainTextJob = new MainTextJob(&part, &composerJob);
    auto seJob = new SignEncryptJob(&composerJob);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const QStringList recipients = {QString::fromLocal8Bit("test@kolab.org")};

    seJob->setContent(mainTextJob->content());
    seJob->setSigningKeys(keys);
    seJob->setCryptoMessageFormat((Kleo::CryptoMessageFormat)cryptoMessageFormat);
    seJob->setRecipients(recipients);
    seJob->setEncryptionKeys(keys);

    if (!error.isEmpty()) {
        QVERIFY(!seJob->exec());
        QCOMPARE(seJob->errorString(), error);
        return;
    }

    VERIFYEXEC(seJob);
    KMime::Content *result = seJob->content();
    QVERIFY(result);
    result->assemble();

    ComposerTestUtil::verifySignatureAndEncryption(result, data.toUtf8(), (Kleo::CryptoMessageFormat)cryptoMessageFormat, false, true);

    delete result;
}

void SignEncryptTest::testContentSubjobChained()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    KMime::Message skeletonMessage;

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    const QStringList recipients = {QString::fromLocal8Bit("test@kolab.org")};

    ComposerJob composerJob;
    auto seJob = new SignEncryptJob(&composerJob);

    seJob->setSigningKeys(keys);
    seJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    seJob->setRecipients(recipients);
    seJob->setEncryptionKeys(keys);
    seJob->setSkeletonMessage(&skeletonMessage);
    QVERIFY(seJob->appendSubjob(tJob));

    VERIFYEXEC(seJob);
    KMime::Content *result = seJob->content();
    QVERIFY(result);
    result->assemble();

    ComposerTestUtil::verifySignatureAndEncryption(result, data, Kleo::OpenPGPMIMEFormat, false, true);

    delete result;
}

void SignEncryptTest::testHeaders()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    ComposerJob composerJob;
    auto seJob = new SignEncryptJob(&composerJob);

    QVERIFY(seJob);

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    const QStringList recipients = {QString::fromLocal8Bit("test@kolab.org")};

    seJob->setContent(content);
    seJob->setSigningKeys(keys);
    seJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    seJob->setRecipients(recipients);
    seJob->setEncryptionKeys(keys);

    VERIFYEXEC(seJob);

    KMime::Content *result = seJob->content();
    QVERIFY(result);
    result->assemble();

    QFile f(u"test"_s);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    const QByteArray encodedContent(result->encodedContent());
    f.write(encodedContent);
    if (!encodedContent.endsWith('\n')) {
        f.write("\n");
    }
    f.close();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), "multipart/encrypted");
    QCOMPARE(result->contentType()->charset(), "UTF-8");
    QCOMPARE(result->contentType()->parameter("protocol"), QString::fromLocal8Bit("application/pgp-encrypted"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}

void SignEncryptTest::testProtectedHeaders_data()
{
    QTest::addColumn<bool>("protectedHeaders");
    QTest::addColumn<bool>("protectedHeadersObvoscate");
    QTest::addColumn<QString>("referenceFile");

    QTest::newRow("simple-obvoscate") << true << true << u"protected_headers-obvoscate.mbox"_s;
    QTest::newRow("simple-non-obvoscate") << true << false << u"protected_headers-non-obvoscate.mbox"_s;
    QTest::newRow("non-protected_headers") << false << false << u"non-protected_headers.mbox"_s;
}

void SignEncryptTest::testProtectedHeaders()
{
    QFETCH(bool, protectedHeaders);
    QFETCH(bool, protectedHeadersObvoscate);
    QFETCH(QString, referenceFile);

    const std::vector<GpgME::Key> &keys = Test::getKeys();

    ComposerJob composerJob;
    auto seJob = new SignEncryptJob(&composerJob);

    QVERIFY(seJob);

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

    seJob->setContent(content);
    seJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    seJob->setRecipients(recipients);
    seJob->setEncryptionKeys(keys);
    seJob->setSkeletonMessage(&skeletonMessage);
    seJob->setProtectedHeaders(protectedHeaders);
    seJob->setProtectedHeadersObvoscate(protectedHeadersObvoscate);

    VERIFYEXEC(seJob);

    if (protectedHeadersObvoscate) {
        QCOMPARE(skeletonMessage.subject()->as7BitString(false), "...");
    } else {
        QCOMPARE(skeletonMessage.subject()->asUnicodeString(), subject);
    }

    KMime::Content *result = seJob->content();
    result->assemble();

    KMime::Content *encPart = Util::findTypeInMessage(result, "application", "octet-stream");
    KMime::Content tempNode;
    {
        QByteArray plainText;
        auto job = QGpgME::openpgp()->decryptVerifyJob();
        auto jobResult = job->exec(encPart->encodedBody(), plainText);

        auto signature = jobResult.second.signatures()[0];

        QCOMPARE(signature.fingerprint(), "1BA323932B3FAA826132C79E8D9860C58F246DE6");
        QCOMPARE(signature.status().code(), 0);

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

#include "moc_signencrypttest.cpp"
