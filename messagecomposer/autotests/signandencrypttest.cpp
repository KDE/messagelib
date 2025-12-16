/*
  SPDX-FileCopyrightText: 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "signandencrypttest.h"
using namespace Qt::Literals::StringLiterals;

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include <QTest>

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/ComposerJob>
#include <MessageComposer/EncryptJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/SignJob>
#include <MessageComposer/TextPart>

#include "setupenv.h"
#include <MimeTreeParser/NodeHelper>

#include <gpgme.h>

QTEST_MAIN(SignAndEncryptTest)

using namespace MessageComposer;

void SignAndEncryptTest::initMain()
{
    gpgme_check_version(nullptr);
}

void SignAndEncryptTest::initTestCase()
{
    Test::setupEnv();
}

void SignAndEncryptTest::testContent()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    ComposerJob composerJob;

    TextPart part;
    part.setWordWrappingEnabled(false);
    part.setCleanPlainText(u"one flew over the cuckoo's nest"_s);

    auto mainTextJob = new MainTextJob(&part, &composerJob);
    auto sJob = new SignJob(&composerJob);
    auto eJob = new EncryptJob(&composerJob);

    QVERIFY(mainTextJob);

    VERIFYEXEC(mainTextJob);

    const QStringList recipients = {u"test@kolab.org"_s};

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

    ComposerTestUtil::verifySignatureAndEncryption(result, u"one flew over the cuckoo's nest"_s.toUtf8(), Kleo::OpenPGPMIMEFormat);

    delete result;
}

void SignAndEncryptTest::testHeaders()
{
    const std::vector<GpgME::Key> &keys = Test::getKeys();

    ComposerJob composerJob;
    auto sJob = new SignJob(&composerJob);
    auto eJob = new EncryptJob(&composerJob);

    QVERIFY(sJob);
    QVERIFY(eJob);

    const QByteArray data(u"one flew over the cuckoo's nest"_s.toUtf8());
    auto content = new KMime::Content;
    content->setBody(data);

    const QStringList recipients = {u"test@kolab.org"_s};

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

    QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentType()->mimeType(), "multipart/encrypted");
    QCOMPARE(result->contentType()->charset(), "UTF-8");
    QCOMPARE(result->contentType()->parameter("protocol"), u"application/pgp-encrypted"_s);
    QCOMPARE(result->contentTransferEncoding()->encoding(), KMime::Headers::CE7Bit);

    delete result;
}

#include "moc_signandencrypttest.cpp"
