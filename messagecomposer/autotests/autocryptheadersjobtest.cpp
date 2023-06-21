/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptheadersjobtest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/AutocryptHeadersJob>
#include <MessageComposer/Composer>
#include <MessageComposer/Util>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/verificationresult.h>

#include <cstdlib>

#include <MessageComposer/TransparentJob>
#include <QTest>

QTEST_MAIN(AutocryptHeadersJobTest)

using namespace MessageComposer;

void AutocryptHeadersJobTest::initTestCase()
{
    Test::setupEnv();
}

void AutocryptHeadersJobTest::testAutocryptHeader()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");
    skeletonMessage.to(true)->from7BitString("Bob <bob@autocrypt.example>");
    skeletonMessage.subject(true)->from7BitString("an Autocrypt header example using Ed25519+Cv25519 key");
    skeletonMessage.date(true)->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    skeletonMessage.messageID(true)->from7BitString("<abe640bb-018d-4f9d-b4d8-1636d6164e22@autocrypt.example>");

    KMime::Content content;
    content.contentType(true)->from7BitString("text/plain");
    content.setBody("This is an example e-mail with Autocrypt header and Ed25519+Cv25519 key (key\nfingerprint: ) as defined in Level 1 revision 1.1.\n");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    auto res = job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);

    auto aJob = new AutocryptHeadersJob(&composer);

    QVERIFY(aJob);

    aJob->setContent(&content);
    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    VERIFYEXEC(aJob);

    skeletonMessage.assemble();
    content.assemble();

    auto referenceFile = QStringLiteral("autcryptheader.mbox");
    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(skeletonMessage.head());
    f.write(content.encodedContent());
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testContentChained()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");
    skeletonMessage.to(true)->from7BitString("Bob <bob@autocrypt.example>");
    skeletonMessage.subject(true)->from7BitString("an Autocrypt header example using Ed25519+Cv25519 key");
    skeletonMessage.date(true)->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    skeletonMessage.messageID(true)->from7BitString("<abe640bb-018d-4f9d-b4d8-1636d6164e22@autocrypt.example>");

    KMime::Content content;
    content.contentType(true)->from7BitString("text/plain");
    content.setBody("This is an example e-mail with Autocrypt header and Ed25519+Cv25519 key (key\nfingerprint: ) as defined in Level 1 revision 1.1.\n");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    auto res = job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);

    auto aJob = new AutocryptHeadersJob(&composer);
    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(&content);

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    skeletonMessage.assemble();
    content.assemble();

    auto referenceFile = QStringLiteral("autcryptheader.mbox");
    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(skeletonMessage.head());
    f.write(content.encodedContent());
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testAutocryptGossipHeader()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");

    KMime::Content content;
    content.setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    job->exec(QStringList({QStringLiteral("bob@autocrypt.example"), QStringLiteral("carol@autocrypt.example")}), false, keys);

    auto aJob = new AutocryptHeadersJob(&composer);

    QVERIFY(aJob);

    aJob->setContent(&content);
    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    VERIFYEXEC(aJob);

    content.contentType(true)->from7BitString("text/plain");
    content.assemble();

    auto referenceFile = QStringLiteral("autocryptgossipheader.mbox");
    Test::compareFile(&content, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testSetGnupgHome()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");

    KMime::Content content;
    content.setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");

    auto exportJob = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    exportJob->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    exportJob->exec(QStringList({QStringLiteral("bob@autocrypt.example"), QStringLiteral("carol@autocrypt.example")}), false, keys);

    QTemporaryDir dir;
    { // test with an empty gnupg Home
        auto aJob = new AutocryptHeadersJob(&composer);
        QVERIFY(aJob);

        aJob->setContent(&content);
        aJob->setSkeletonMessage(&skeletonMessage);
        aJob->setSenderKey(ownKeys[0]);
        aJob->setPreferEncrypted(true);
        aJob->setGossipKeys(keys);
        aJob->setGnupgHome(dir.path());
        QCOMPARE(aJob->exec(), false);
    }

    // Populate Keyring with needed keys.
    Test::populateKeyring(dir.path(), ownKeys[0]);
    for (const auto &key : keys) {
        Test::populateKeyring(dir.path(), key);
    }
    auto aJob = new AutocryptHeadersJob(&composer);
    QVERIFY(aJob);

    aJob->setContent(&content);
    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    aJob->setGnupgHome(dir.path());

    VERIFYEXEC(aJob);

    content.contentType(true)->from7BitString("text/plain");
    content.assemble();

    auto referenceFile = QStringLiteral("autocryptgossipheader.mbox");
    Test::compareFile(&content, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testStripSenderKey()
{
    Composer composer;

    KMime::Message skeletonMessage;
    skeletonMessage.from(true)->from7BitString("Alice <alice@autocrypt.example>");

    KMime::Content content;
    content.setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    job->exec(QStringList({QStringLiteral("bob@autocrypt.example"), QStringLiteral("carol@autocrypt.example")}), false, keys);
    keys.push_back(ownKeys[0]);

    auto aJob = new AutocryptHeadersJob(&composer);

    QVERIFY(aJob);

    aJob->setContent(&content);
    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    VERIFYEXEC(aJob);

    content.contentType(true)->from7BitString("text/plain");
    content.assemble();

    auto referenceFile = QStringLiteral("autocryptgossipheader.mbox");
    Test::compareFile(&content, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

#include "moc_autocryptheadersjobtest.cpp"
