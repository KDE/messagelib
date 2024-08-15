/*
  SPDX-FileCopyrightText: 2022 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptresolvercoretest.h"

#include <MessageComposer/AutocryptKeyResolverCore>

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <Libkleo/KeyCache>

#include <QStandardPaths>
#include <QTest>

QTEST_MAIN(AutocryptKeyResolverCoreTest)

using namespace MessageComposer;

void AutocryptKeyResolverCoreTest::initTestCase()
{
    Test::setupFullEnv();

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    autocryptDir = QDir(genericDataLocation.filePath(QStringLiteral("autocrypt")));
}

void AutocryptKeyResolverCoreTest::init()
{
    autocryptDir.removeRecursively();
    autocryptDir.mkpath(QStringLiteral("."));

    // hold a reference to the key cache to avoid rebuilding while the test is running
    mKeyCache = Kleo::KeyCache::instance();
    // make sure that the key cache has been populated
    (void)mKeyCache->keys();
}

void AutocryptKeyResolverCoreTest::cleanup()
{
    autocryptDir.removeRecursively();
    QVERIFY(mKeyCache.use_count() == 1);
    mKeyCache.reset();
}

void AutocryptKeyResolverCoreTest::testAutocryptKeyResolver()
{
    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"), QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(autocryptDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
    QVERIFY(file2.copy(autocryptDir.filePath(QStringLiteral("recipient2%40autocrypt.example.json"))));

    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    resolver.setRecipients(recipients);

    const auto result = resolver.resolve();

    QCOMPARE(result.flags & Kleo::KeyResolverCore::ResolvedMask, Kleo::KeyResolverCore::AllResolved);
    QCOMPARE(result.flags & Kleo::KeyResolverCore::ProtocolsMask, Kleo::KeyResolverCore::OpenPGPOnly);
    QCOMPARE(result.solution.protocol, GpgME::OpenPGP);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[0]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[0]), true);
    QCOMPARE(resolver.isGossipKey(recipients[0]), true);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[1]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[1]), true);
    QCOMPARE(resolver.isGossipKey(recipients[1]), false);
}

void AutocryptKeyResolverCoreTest::testAutocryptKeyResolverSkipSender()
{
    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"), QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(autocryptDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
    QVERIFY(file2.copy(autocryptDir.filePath(QStringLiteral("recipient2%40autocrypt.example.json"))));

    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    resolver.setSender(QStringLiteral("recipient@autocrypt.example"));
    resolver.setRecipients(recipients);

    const auto result = resolver.resolve();

    QCOMPARE(result.flags & Kleo::KeyResolverCore::ResolvedMask, Kleo::KeyResolverCore::SomeUnresolved);
    QCOMPARE(result.flags & Kleo::KeyResolverCore::ProtocolsMask, Kleo::KeyResolverCore::OpenPGPOnly);
    QCOMPARE(result.solution.protocol, GpgME::OpenPGP);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[0]).size(), 0);
    QCOMPARE(resolver.isAutocryptKey(recipients[0]), false);
    QCOMPARE(resolver.isGossipKey(recipients[0]), false);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[1]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[1]), true);
    QCOMPARE(resolver.isGossipKey(recipients[1]), false);
}

void AutocryptKeyResolverCoreTest::testAutocryptKeyResolverUnresolved()
{
    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"),
                              QStringLiteral("recipient2@autocrypt.example"),
                              QStringLiteral("unresolved@test.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(autocryptDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
    QVERIFY(file2.copy(autocryptDir.filePath(QStringLiteral("recipient2%40autocrypt.example.json"))));

    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    resolver.setRecipients(recipients);

    const auto result = resolver.resolve();

    QCOMPARE(result.flags & Kleo::KeyResolverCore::ResolvedMask, Kleo::KeyResolverCore::SomeUnresolved);
    QCOMPARE(result.flags & Kleo::KeyResolverCore::ProtocolsMask, Kleo::KeyResolverCore::OpenPGPOnly);
    QCOMPARE(result.solution.protocol, GpgME::OpenPGP);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[0]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[0]), true);
    QCOMPARE(resolver.isGossipKey(recipients[0]), true);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[1]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[1]), true);
    QCOMPARE(resolver.isGossipKey(recipients[1]), false);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[2]).size(), 0);
    QCOMPARE(resolver.isAutocryptKey(recipients[2]), false);
    QCOMPARE(resolver.isGossipKey(recipients[2]), false);
}

void AutocryptKeyResolverCoreTest::testAutocryptKeyResolverPreferNormal()
{
    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"), QStringLiteral("test@kolab.org")};
    auto key = mKeyCache->findBestByMailBox("test@kolab.org", GpgME::OpenPGP, Kleo::KeyCache::KeyUsage::Encrypt);

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(autocryptDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
    QVERIFY(file2.copy(autocryptDir.filePath(QStringLiteral("test%40kolab.org.json"))));

    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    resolver.setRecipients(recipients);

    const auto result = resolver.resolve();

    QCOMPARE(result.flags & Kleo::KeyResolverCore::ResolvedMask, Kleo::KeyResolverCore::AllResolved);
    QCOMPARE(result.flags & Kleo::KeyResolverCore::ProtocolsMask, Kleo::KeyResolverCore::OpenPGPOnly);
    QCOMPARE(result.solution.protocol, GpgME::OpenPGP);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[0]).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipients[0]), true);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[1]).size(), 1);
    QCOMPARE(result.solution.encryptionKeys.value(recipients[1]).at(0).primaryFingerprint(), key.primaryFingerprint());
    QCOMPARE(resolver.isAutocryptKey(recipients[1]), false);
}

void AutocryptKeyResolverCoreTest::testNormalKeyResolver()
{
    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    QString recipient(QStringLiteral("test@kolab.org"));
    resolver.setRecipients({recipient});

    const auto result = resolver.resolve();

    QCOMPARE(result.flags & Kleo::KeyResolverCore::ResolvedMask, Kleo::KeyResolverCore::AllResolved);
    QCOMPARE(result.flags & Kleo::KeyResolverCore::ProtocolsMask, Kleo::KeyResolverCore::OpenPGPOnly);
    QCOMPARE(result.solution.protocol, GpgME::OpenPGP);
    QCOMPARE(result.solution.encryptionKeys.value(recipient).size(), 1);
    QCOMPARE(resolver.isAutocryptKey(recipient), false);
    QCOMPARE(resolver.isGossipKey(recipient), false);
}

#include "moc_autocryptresolvercoretest.cpp"
