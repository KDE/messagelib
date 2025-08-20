/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "keyresolvertest.h"
using namespace Qt::Literals::StringLiterals;

#include "../src/composer/keyresolver.h"

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <Libkleo/ExpiryChecker>
#include <Libkleo/ExpiryCheckerSettings>

#include <QStandardPaths>
#include <QTest>

#include <gpgme.h>

QTEST_MAIN(KeyResolverTest)

using namespace MessageComposer;
using namespace Kleo;

void KeyResolverTest::initMain()
{
    gpgme_check_version(nullptr);
}

void KeyResolverTest::initTestCase()
{
    Test::setupFullEnv();

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    baseDir = QDir(genericDataLocation.filePath(u"autocrypt"_s));
}

void KeyResolverTest::init()
{
    baseDir.removeRecursively();
    baseDir.mkpath(u"."_s);
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

    QStringList recipients = {u"recipient@autocrypt.example"_s, QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient%40autocrypt.example.json"_s);
    QVERIFY(file1.copy(baseDir.filePath(u"recipient%40autocrypt.example.json"_s)));
    QFile file2(QLatin1StringView(MAIL_DATA_DIR) + u"/autocrypt/recipient2%40autocrypt.example.json"_s);
    QVERIFY(file2.copy(baseDir.filePath(u"recipient2%40autocrypt.example.json"_s)));

    QCOMPARE(keyResolver.setEncryptToSelfKeys({QString::fromLatin1(keys[0].primaryFingerprint())}), ResolverResult::Ok);
    QCOMPARE(keyResolver.setSigningKeys({QString::fromLatin1(keys[0].primaryFingerprint())}), ResolverResult::Ok);
    keyResolver.setAutocryptEnabled(true);
    keyResolver.setPrimaryRecipients(recipients);

    bool signSomething(false), encryptSomething(true);
    const auto kpgpResult = keyResolver.resolveAllKeys(signSomething, encryptSomething);
    QCOMPARE(kpgpResult, ResolverResult::Ok);
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
