/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signandencrypttest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include <QTest>

#include <kmime/kmime_content.h>

#include <Libkleo/Enum>

#include <MessageComposer/Composer>
#include <MessageComposer/EncryptJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/SignJob>
#include <MessageComposer/TextPart>

#include <MimeTreeParser/NodeHelper>
#include <setupenv.h>

#include <MessageCore/NodeHelper>

QTEST_MAIN(SignAndEncryptTest)

using namespace MessageComposer;

void SignAndEncryptTest::initTestCase()
{
    Test::setupEnv();
}

void SignAndEncryptTest::testContent()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;

    const QVector<QByteArray> charsets = {"us-ascii"};
    composer.globalPart()->setCharsets(charsets);

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(QStringLiteral("one flew over the cuckoo's nest"));

    auto mainTextJob = new MainTextJob(&part, &composer);
    auto sJob = new SignJob(&composer);
    auto eJob = new EncryptJob(&composer);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    sJob->setContent(mainTextJob->content());
    sJob->setSigningKeys(keys);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    eJob->appendSubjob(sJob);

    VERIFYEXEC(eJob);

    KMime::Content *result = eJob->content();
    QVERIFY(result);
    result->assemble();

    ComposerTestUtil::verifySignatureAndEncryption(result, QStringLiteral("one flew over the cuckoo's nest").toUtf8(), Kleo::OpenPGPMIMEFormat);

    delete result;
}

void SignAndEncryptTest::testHeaders()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    Composer composer;
    auto sJob = new SignJob(&composer);
    auto eJob = new EncryptJob(&composer);

    QVERIFY(sJob);
    QVERIFY(eJob);

    const QByteArray data(QStringLiteral("one flew over the cuckoo's nest").toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    const QStringList recipients = {QStringLiteral("test@kolab.org")};

    sJob->setContent(content);
    sJob->setSigningKeys(keys);
    sJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);

    eJob->setCryptoMessageFormat(Kleo::OpenPGPMIMEFormat);
    eJob->setRecipients(recipients);
    eJob->setEncryptionKeys(keys);

    eJob->appendSubjob(sJob);

    VERIFYEXEC(eJob);

    KMime::Content *result = eJob->content();
    QVERIFY(result);
    result->assemble();

    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), "multipart/encrypted");
    QCOMPARE(result->contentType()->charset(), "ISO-8859-1");
    QCOMPARE(result->contentType()->parameter(QStringLiteral("protocol")), QStringLiteral("application/pgp-encrypted"));
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}
