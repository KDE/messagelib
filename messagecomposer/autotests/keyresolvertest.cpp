/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "keyresolvertest.h"

#include "../src/composer/keyresolver.h"

#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <AkonadiCore/Control>
#include <qtest_akonadi.h>

#include <QDebug>
#include <QStandardPaths>
#include <QTest>

QTEST_AKONADIMAIN(KeyResolverTest)

using namespace MessageComposer;
using namespace Kleo;

void KeyResolverTest::initTestCase()
{
    qputenv("LC_ALL", "C");

    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    baseDir = QDir(genericDataLocation.filePath(QStringLiteral("autocrypt")));
    AkonadiTest::checkTestIsIsolated();
    Akonadi::Control::start();

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
    const std::vector<GpgME::Key> &keys = Test::getKeys();
    KeyResolver keyResolver(true, false, true, Kleo::OpenPGPMIMEFormat, 0, 0, 0, 0, 0, 0);

    QStringList recipients = {QStringLiteral("recipient@autocrypt.example"), QStringLiteral("recipient2@autocrypt.example")};

    QFile file1(QLatin1String(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient%40autocrypt.example.json"));
    QVERIFY(file1.copy(baseDir.filePath(QStringLiteral("recipient%40autocrypt.example.json"))));
    QFile file2(QLatin1String(MAIL_DATA_DIR) + QStringLiteral("/autocrypt/recipient2%40autocrypt.example.json"));
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
