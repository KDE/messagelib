/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "autocryptstoragetest.h"

#include "autocrypt/autocryptstorage.h"
#include "autocrypt/autocryptstorage_p.h"

#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTest>

using namespace MessageCore;

QTEST_MAIN(AutocryptStorageTest)

void AutocryptStorageTest::initTestCase()
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    baseDir = QDir(genericDataLocation.filePath(QStringLiteral("autocrypt")));
}

void AutocryptStorageTest::init()
{
    baseDir.removeRecursively();
    baseDir.mkpath(QStringLiteral("."));
}

void AutocryptStorageTest::cleanup()
{
    baseDir.removeRecursively();
    auto storage = AutocryptStorage::self();
    storage->d_func()->recipients.clear();
}

void AutocryptStorageTest::test_uniqness()
{
    auto storage = AutocryptStorage::self();
    auto storage2 = AutocryptStorage::self();

    QCOMPARE(storage.data(), storage2.data());
}

void AutocryptStorageTest::test_unknown()
{
    auto storage = AutocryptStorage::self();

    QCOMPARE(storage->getRecipient("unknown@autocrypt.example"), nullptr);
}

void AutocryptStorageTest::test_existing()
{
    auto storage = AutocryptStorage::self();
    QFile file(QLatin1StringView(DATA_DIR) + QStringLiteral("/autocrypt/empty.json"));
    QVERIFY(file.copy(baseDir.path() + QStringLiteral("/empty%40autocrypt.example.json")));

    const auto recipient = storage->getRecipient("empty@autocrypt.example");
    const auto recipient2 = storage->getRecipient("empty@autocrypt.example");
    QVERIFY(recipient);
    QCOMPARE(recipient->hasChanged(), false);
    QCOMPARE(recipient.data(), recipient2.data());
}

void AutocryptStorageTest::test_store()
{
    auto storage = AutocryptStorage::self();
    QFile data(QLatin1StringView(DATA_DIR) + QStringLiteral("/autocrypt/empty.json"));

    const QString fileName(QStringLiteral("store%40autocrypt.example.json"));
    QFile file(baseDir.filePath(fileName));

    QVERIFY(data.copy(baseDir.filePath(fileName)));

    const auto recipient = storage->getRecipient("store@autocrypt.example");
    QVERIFY(recipient);

    baseDir.remove(fileName);
    QCOMPARE(baseDir.exists(fileName), false);

    // Save file, when file does not exist
    storage->save();
    QCOMPARE(baseDir.exists(fileName), true);

    // Don't save when file exists and changed flag is false

    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write("something");
    file.close();

    storage->save();

    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(file.readAll(), "something");
    file.close();

    // save when changed flag is true
    recipient->setChangedFlag(true);
    storage->save();
    QCOMPARE(baseDir.exists(fileName), true);
    QCOMPARE(recipient->hasChanged(), false);

    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    QVERIFY(data.open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(file.readAll().trimmed(), data.readAll().trimmed());
    file.close();
    data.close();
}

void AutocryptStorageTest::test_addRecipient()
{
    const QString fileName(QStringLiteral("add%40autocrypt.example.json"));
    baseDir.remove(fileName);

    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->getRecipient("add@autocrypt.example"), nullptr);
    const auto recipient = storage->addRecipient("add@autocrypt.example");
    QVERIFY(recipient);

    const auto document = QJsonDocument::fromJson(recipient->toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(QStringLiteral("addr")).toString(), QStringLiteral("add@autocrypt.example"));
}

void AutocryptStorageTest::test_deleteRecipient()
{
    const QByteArray addr("delete@autocrypt.example");
    const QString fileName(QStringLiteral("delete%40autocrypt.example.json"));
    auto storage = AutocryptStorage::self();
    QVERIFY(storage->addRecipient(addr));
    QVERIFY(storage->getRecipient(addr));
    storage->save();
    QCOMPARE(baseDir.exists(fileName), true);

    storage->deleteRecipient(addr);
    QCOMPARE(baseDir.exists(fileName), false);
    QCOMPARE(storage->getRecipient(addr), nullptr);
}

void AutocryptStorageTest::test_create_basedir()
{
    const QByteArray addr("recipient@autocrypt.example");
    const QString fileName(QStringLiteral("recipient%40autocrypt.example.json"));
    auto storage = AutocryptStorage::self();
    QVERIFY(storage->addRecipient(addr));
    baseDir.removeRecursively();
    QCOMPARE(baseDir.exists(), false);
    storage->save();
    QCOMPARE(baseDir.exists(fileName), true);
}

#include "moc_autocryptstoragetest.cpp"
