/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signencrypttest.h"

#include <QTest>
#include "qtest_messagecomposer.h"
#include "cryptofunctions.h"

#include <kmime/kmime_content.h>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/SignEncryptJob>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/TransparentJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/TextPart>

#include <setupenv.h>

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
    QTest::newRow("SMIMEFormat") << (int)Kleo::SMIMEFormat << QStringLiteral("Not implemented");
    QTest::newRow("SMIMEOpaqueFormat") << (int)Kleo::SMIMEOpaqueFormat << QStringLiteral("Not implemented");
}

void SignEncryptTest::testContent()
{
    QFETCH(int, cryptoMessageFormat);
    QFETCH(QString, error);

    const std::vector< GpgME::Key > &keys = Test::getKeys();
    const QString data(QString::fromLocal8Bit("one flew over the cuckoo's nest"));

    Composer composer;

    const QVector<QByteArray> charsets = {"us-ascii"};
    composer.globalPart()->setCharsets(charsets);

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(data);

    auto mainTextJob = new MainTextJob(&part, &composer);
    auto seJob = new SignEncryptJob(&composer);

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

    ComposerTestUtil::verifySignatureAndEncryption(
        result,
        data.toUtf8(),
        (Kleo::CryptoMessageFormat)cryptoMessageFormat,
        false,
        true);

    delete result;
}

void SignEncryptTest::testContentSubjobChained()
{
    const std::vector< GpgME::Key > &keys = Test::getKeys();

    const QByteArray data(QString::fromLocal8Bit("one flew over the cuckoo's nest").toUtf8());
    KMime::Message skeletonMessage;

    auto content = new KMime::Content;
    content->contentType(true)->setMimeType("text/plain");
    content->setBody(data);

    auto tJob = new TransparentJob;
    tJob->setContent(content);

    const QStringList recipients = {QString::fromLocal8Bit("test@kolab.org")};

    Composer composer;
    auto seJob = new SignEncryptJob(&composer);

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

    ComposerTestUtil::verifySignatureAndEncryption(
        result,
        data,
        Kleo::OpenPGPMIMEFormat,
        false,
        true);

    delete result;
}

void SignEncryptTest::testHeaders()
{
    const std::vector< GpgME::Key > &keys = Test::getKeys();

    Composer composer;
    auto seJob = new SignEncryptJob(&composer);

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

    QFile f(QStringLiteral("test"));
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    const QByteArray encodedContent(result->encodedContent());
    f.write(encodedContent);
    if (!encodedContent.endsWith('\n')) {
        f.write("\n");
    }
    f.close();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), "multipart/encrypted");
    QCOMPARE(result->contentType()->charset(), "ISO-8859-1");
    QCOMPARE(result->contentType()->parameter(QString::fromLocal8Bit("protocol")), QString::fromLocal8Bit("application/pgp-encrypted"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}
