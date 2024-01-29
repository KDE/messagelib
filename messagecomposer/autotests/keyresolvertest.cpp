/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "keyresolvertest.h"

#include "../src/composer/keyresolver.h"

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <Libkleo/ExpiryChecker>
#include <Libkleo/ExpiryCheckerSettings>

#include <QStandardPaths>
#include <QTest>

QTEST_MAIN(KeyResolverTest)

using namespace MessageComposer;
using namespace Kleo;

void KeyResolverTest::initTestCase()
{
    Test::setupFullEnv();

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    baseDir = QDir(genericDataLocation.filePath(QStringLiteral("autocrypt")));
}

void KeyResolverTest::init()
{
    baseDir.removeRecursively();
    baseDir.mkpath(QStringLiteral("."));
}

void KeyResolverTest::cleanup()
{
    baseDir.removeRecursively();
}

void KeyResolverTest::testAutocrypt()
{
    using days = Kleo::chrono::days;

    const std::vector<GpgME::Key> &keys = Test::getKeys();
    std::shared_ptr<ExpiryChecker> expiryChecker{new ExpiryChecker{ExpiryCheckerSettings{days{0}, days{0}, days{0}, days{0}}}};
    KeyResolver keyResolver(true, false, true, Kleo::OpenPGPMIMEFormat, expiryChecker);
    keyResolver.setAkonadiLookupEnabled(false);

    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"), QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(baseDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
    QVERIFY(file2.copy(baseDir.filePath(QStringLiteral("recipient2%40autocrypt.example.json"))));

    QCOMPARE(keyResolver.setEncryptToSelfKeys({QString::fromLatin1(keys[0].primaryFingerprint())}), Kleo::Ok);
    QCOMPARE(keyResolver.setSigningKeys({QString::fromLatin1(keys[0].primaryFingerprint())}), Kleo::Ok);
    keyResolver.setAutocryptEnabled(true);
    keyResolver.setPrimaryRecipients(recipients);

    bool signSomething(false), encryptSomething(true);
    const auto kpgpResult = keyResolver.resolveAllKeys(signSomething, encryptSomething);
    QCOMPARE(kpgpResult, Kleo::Ok);
    QVERIFY(!signSomething);
    QVERIFY(encryptSomething);
    const auto items = keyResolver.encryptionItems(Kleo::OpenPGPMIMEFormat);
    const auto map = keyResolver.useAutocrypt();
    QCOMPARE(items.size(), 1);
    QCOMPARE(map.size(), 2);
    QCOMPARE(items[0].recipients, recipients);
    QCOMPARE(items[0].keys.size(), 3);
}

#include "moc_keyresolvertest.cpp"
