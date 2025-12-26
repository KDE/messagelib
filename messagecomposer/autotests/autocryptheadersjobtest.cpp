/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptheadersjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <KMime/Content>

#include <Libkleo/Enum>

#include <MessageComposer/AutocryptHeadersJob>
#include <MessageComposer/ComposerJob>
#include <MessageComposer/TransparentJob>
#include <MessageComposer/Util>

#include <QGpgME/KeyListJob>
#include <QGpgME/Protocol>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/keylistresult.h>
#include <gpgme++/verificationresult.h>

#include <gpgme.h>

#include <cstdlib>

#include <QTest>

QTEST_MAIN(AutocryptHeadersJobTest)

using namespace MessageComposer;

void AutocryptHeadersJobTest::initMain()
{
    gpgme_check_version(nullptr);
}

void AutocryptHeadersJobTest::initTestCase()
{
    Test::setupEnv();
}

void AutocryptHeadersJobTest::testAutocryptHeader()
{
    ComposerJob composerJob;

    KMime::Message skeletonMessage;
    skeletonMessage.from(KMime::CreatePolicy::Create)->from7BitString("Alice <alice@autocrypt.example>");
    skeletonMessage.to(KMime::CreatePolicy::Create)->from7BitString("Bob <bob@autocrypt.example>");
    skeletonMessage.subject(KMime::CreatePolicy::Create)->from7BitString("an Autocrypt header example using Ed25519+Cv25519 key");
    skeletonMessage.date(KMime::CreatePolicy::Create)->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    skeletonMessage.messageID(KMime::CreatePolicy::Create)->from7BitString("<abe640bb-018d-4f9d-b4d8-1636d6164e22@autocrypt.example>");

    auto content = std::make_unique<KMime::Content>();
    content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");
    content->setBody("This is an example e-mail with Autocrypt header and Ed25519+Cv25519 key (key\nfingerprint: ) as defined in Level 1 revision 1.1.\n");
    content->assemble();
    const auto encodedContent = content->encodedContent();

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    auto res = job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);

    auto aJob = new AutocryptHeadersJob(&composerJob);

    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(std::move(content));

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    skeletonMessage.assemble();

    auto referenceFile = u"autcryptheader.mbox"_s;
    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(skeletonMessage.head());
    f.write(encodedContent);
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testContentChained()
{
    ComposerJob composerJob;

    KMime::Message skeletonMessage;
    skeletonMessage.from(KMime::CreatePolicy::Create)->from7BitString("Alice <alice@autocrypt.example>");
    skeletonMessage.to(KMime::CreatePolicy::Create)->from7BitString("Bob <bob@autocrypt.example>");
    skeletonMessage.subject(KMime::CreatePolicy::Create)->from7BitString("an Autocrypt header example using Ed25519+Cv25519 key");
    skeletonMessage.date(KMime::CreatePolicy::Create)->from7BitString("Tue, 22 Jan 2019 12:56:25 +0100");
    skeletonMessage.messageID(KMime::CreatePolicy::Create)->from7BitString("<abe640bb-018d-4f9d-b4d8-1636d6164e22@autocrypt.example>");

    auto content = std::make_unique<KMime::Content>();
    content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");
    content->setBody("This is an example e-mail with Autocrypt header and Ed25519+Cv25519 key (key\nfingerprint: ) as defined in Level 1 revision 1.1.\n");
    content->assemble();
    const auto encodedContent = content->encodedContent();

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    auto res = job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);

    auto aJob = new AutocryptHeadersJob(&composerJob);
    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(std::move(content));

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    skeletonMessage.assemble();

    auto referenceFile = u"autcryptheader.mbox"_s;
    QFile f(referenceFile);
    QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(skeletonMessage.head());
    f.write(encodedContent);
    f.close();

    Test::compareFile(referenceFile, QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testAutocryptGossipHeader()
{
    ComposerJob composerJob;

    KMime::Message skeletonMessage;
    skeletonMessage.from(KMime::CreatePolicy::Create)->from7BitString("Alice <alice@autocrypt.example>");

    auto content = std::make_unique<KMime::Content>();
    content->setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");
    content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    job->exec(QStringList({u"bob@autocrypt.example"_s, QStringLiteral("carol@autocrypt.example")}), false, keys);

    auto aJob = new AutocryptHeadersJob(&composerJob);

    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(std::move(content));

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    auto referenceFile = u"autocryptgossipheader.mbox"_s;
    auto result = aJob->takeContent();
    result->assemble();
    Test::compareFile(result.get(), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testSetGnupgHome()
{
    ComposerJob composerJob;

    KMime::Message skeletonMessage;
    skeletonMessage.from(KMime::CreatePolicy::Create)->from7BitString("Alice <alice@autocrypt.example>");

    auto content = std::make_unique<KMime::Content>();
    content->setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");
    content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");

    auto exportJob = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    exportJob->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    exportJob->exec(QStringList({u"bob@autocrypt.example"_s, QStringLiteral("carol@autocrypt.example")}), false, keys);

    QTemporaryDir dir;
    { // test with an empty gnupg Home
        auto content = std::make_unique<KMime::Content>();
        content->setBody(
            "Hi Bob and Carol,\n"
            "\n"
            "I wanted to introduce the two of you to each other.\n"
            "\n"
            "I hope you are both doing well!  You can now both \"reply all\" here,\n"
            "and the thread will remain encrypted.\n");
        content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");

        auto aJob = new AutocryptHeadersJob(&composerJob);
        QVERIFY(aJob);

        auto tJob = new TransparentJob;
        tJob->setContent(std::move(content));

        aJob->setSkeletonMessage(&skeletonMessage);
        aJob->setSenderKey(ownKeys[0]);
        aJob->setPreferEncrypted(true);
        aJob->setGossipKeys(keys);
        aJob->setGnupgHome(dir.path());
        QVERIFY(aJob->appendSubjob(tJob));
        QCOMPARE(aJob->exec(), false);
    }

    // Populate Keyring with needed keys.
    Test::populateKeyring(dir.path(), ownKeys[0]);
    for (const auto &key : keys) {
        Test::populateKeyring(dir.path(), key);
    }
    auto aJob = new AutocryptHeadersJob(&composerJob);
    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(std::move(content));

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    aJob->setGnupgHome(dir.path());
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    auto referenceFile = u"autocryptgossipheader.mbox"_s;
    auto result = aJob->takeContent();
    result->assemble();
    Test::compareFile(result.get(), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

void AutocryptHeadersJobTest::testStripSenderKey()
{
    ComposerJob composerJob;

    KMime::Message skeletonMessage;
    skeletonMessage.from(KMime::CreatePolicy::Create)->from7BitString("Alice <alice@autocrypt.example>");

    auto content = std::make_unique<KMime::Content>();
    content->setBody(
        "Hi Bob and Carol,\n"
        "\n"
        "I wanted to introduce the two of you to each other.\n"
        "\n"
        "I hope you are both doing well!  You can now both \"reply all\" here,\n"
        "and the thread will remain encrypted.\n");
    content->contentType(KMime::CreatePolicy::Create)->from7BitString("text/plain");

    auto job = QGpgME::openpgp()->keyListJob(false);
    std::vector<GpgME::Key> ownKeys;
    job->exec(QStringList(QString::fromLatin1(skeletonMessage.from()[0].addresses()[0])), false, ownKeys);
    std::vector<GpgME::Key> keys;
    job->exec(QStringList({u"bob@autocrypt.example"_s, QStringLiteral("carol@autocrypt.example")}), false, keys);
    keys.push_back(ownKeys[0]);

    auto aJob = new AutocryptHeadersJob(&composerJob);

    QVERIFY(aJob);

    auto tJob = new TransparentJob;
    tJob->setContent(std::move(content));

    aJob->setSkeletonMessage(&skeletonMessage);
    aJob->setSenderKey(ownKeys[0]);
    aJob->setPreferEncrypted(true);
    aJob->setGossipKeys(keys);
    QVERIFY(aJob->appendSubjob(tJob));
    VERIFYEXEC(aJob);

    auto referenceFile = u"autocryptgossipheader.mbox"_s;
    auto result = aJob->takeContent();
    result->assemble();
    Test::compareFile(result.get(), QStringLiteral(MAIL_DATA_DIR "/") + referenceFile);
}

#include "moc_autocryptheadersjobtest.cpp"
