/*
  SPDX-FileCopyrightText: 2022 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptresolvercoretest.h"
using namespace Qt::Literals::StringLiterals;

#include <MessageComposer/AutocryptKeyResolverCore>

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <Libkleo/KeyCache>

#include <QStandardPaths>
#include <QTest>

#include <gpgme.h>

QTEST_MAIN(AutocryptKeyResolverCoreTest)

using namespace MessageComposer;

void AutocryptKeyResolverCoreTest::initMain()
{
    gpgme_check_version(nullptr);
}

void AutocryptKeyResolverCoreTest::initTestCase()
{
    Test::setupFullEnv();

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    autocryptDir = QDir(genericDataLocation.filePath(u"autocrypt"_s));
}

void AutocryptKeyResolverCoreTest::init()
{
    autocryptDir.removeRecursively();
    autocryptDir.mkpath(u"."_s);

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
    QStringList recipients = {u"recipient@autocrypt.example"_s, QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient%40autocrypt.example.json"_s);
    QVERIFY(file1.copy(autocryptDir.filePath(u"recipient%40autocrypt.example.json"_s)));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient2%40autocrypt.example.json"_s);
    QVERIFY(file2.copy(autocryptDir.filePath(u"recipient2%40autocrypt.example.json"_s)));

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
    QStringList recipients = {u"recipient@autocrypt.example"_s, QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient%40autocrypt.example.json"_s);
    QVERIFY(file1.copy(autocryptDir.filePath(u"recipient%40autocrypt.example.json"_s)));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient2%40autocrypt.example.json"_s);
    QVERIFY(file2.copy(autocryptDir.filePath(u"recipient2%40autocrypt.example.json"_s)));

    AutocryptKeyResolverCore resolver(/*encrypt=*/true, /*sign=*/false);
    resolver.setSender(u"recipient@autocrypt.example"_s);
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
    QStringList recipients = {u"recipient@autocrypt.example"_s, u"recipient2@autocrypt.example"_s, u"unresolved@test.example"_s};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient%40autocrypt.example.json"_s);
    QVERIFY(file1.copy(autocryptDir.filePath(u"recipient%40autocrypt.example.json"_s)));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient2%40autocrypt.example.json"_s);
    QVERIFY(file2.copy(autocryptDir.filePath(u"recipient2%40autocrypt.example.json"_s)));

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
    QStringList recipients = {u"recipient@autocrypt.example"_s, QStringLiteral("test@kolab.org")};
    auto key = mKeyCache->findBestByMailBox("test@kolab.org", GpgME::OpenPGP, Kleo::KeyCache::KeyUsage::Encrypt);

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient%40autocrypt.example.json"_s);
    QVERIFY(file1.copy(autocryptDir.filePath(u"recipient%40autocrypt.example.json"_s)));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient2%40autocrypt.example.json"_s);
    QVERIFY(file2.copy(autocryptDir.filePath(u"test%40kolab.org.json"_s)));

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
    QString recipient(u"test@kolab.org"_s);
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
