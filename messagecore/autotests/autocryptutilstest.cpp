/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "autocryptutilstest.h"
using namespace Qt::Literals::StringLiterals;

#include "autocrypt/autocryptrecipient_p.h"
#include "autocrypt/autocryptstorage.h"
#include "autocrypt/autocryptstorage_p.h"
#include "autocrypt/autocryptutils.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <QFile>
#include <QStandardPaths>
#include <QTest>

using namespace MessageCore;

QTEST_GUILESS_MAIN(AutocryptUtilsTest)

static std::shared_ptr<KMime::Message> readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1StringView(MAIL_DATA_DIR) + u'/' + mailFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    std::shared_ptr<KMime::Message> msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}

void AutocryptUtilsTest::initTestCase()
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
    const QDir genericDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    baseDir = QDir(genericDataLocation.filePath(u"autocrypt"_s));
}

void AutocryptUtilsTest::init()
{
    baseDir.removeRecursively();
    baseDir.mkpath(u"."_s);
}

void AutocryptUtilsTest::cleanup()
{
    baseDir.removeRecursively();
    auto storage = AutocryptStorage::self();
    storage->d_func()->recipients.clear();
}

void AutocryptUtilsTest::test_header()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.get());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.get());

    processAutocryptfromMail(gossipMixin);

    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.keys().size(), 1);

    const QByteArray addr("alice@autocrypt.example");
    QVERIFY2(storage->getRecipient(addr), qPrintable(u"storage missing %1"_s.arg(QString::fromLatin1(addr))));

    QCOMPARE(storage->getRecipient(addr)->gpgKey().isNull(), false);
    QCOMPARE(storage->getRecipient(addr)->gossipKey().isNull(), true);

    QFile data(QLatin1StringView(DATA_DIR) + u"/autocrypt/alice.json"_s);
    QVERIFY(data.open(QIODevice::ReadOnly | QIODevice::Text));
    QCOMPARE(storage->getRecipient(addr)->toJson(QJsonDocument::Compact), data.readAll().trimmed());
    data.close();
}

void AutocryptUtilsTest::test_gossip()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/gossipheader.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.get());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.get());

    processAutocryptfromMail(gossipMixin);

    QSet<QByteArray> expectedKeys;
    expectedKeys << "alice@autocrypt.example"
                 << "bob@autocrypt.example"
                 << "carol@autocrypt.example";

    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.keys().size(), expectedKeys.size());

    for (const auto &addr : std::as_const(expectedKeys)) {
        QVERIFY2(storage->getRecipient(addr), qPrintable(u"storage missing %1"_s.arg(QString::fromLatin1(addr))));
    }

    QCOMPARE(storage->getRecipient("alice@autocrypt.example")->gossipKey().isNull(), true);
    QCOMPARE(storage->getRecipient("bob@autocrypt.example")->gpgKey().isNull(), true);
    QCOMPARE(storage->getRecipient("bob@autocrypt.example")->gossipKey().isNull(), false);
    QCOMPARE(storage->getRecipient("carol@autocrypt.example")->gpgKey().isNull(), true);
    QCOMPARE(storage->getRecipient("carol@autocrypt.example")->gossipKey().isNull(), false);
}

void AutocryptUtilsTest::test_gossipIgnoreNonExcrypted()
{
    // Only parse Autocrypt-Gossip headers in the encrypted payload.

    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/gossipheader-nonencrypted.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.get());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.get());

    processAutocryptfromMail(gossipMixin);

    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.isEmpty(), true);
}

void AutocryptUtilsTest::test_draft()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/draft.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.get());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.get());

    processAutocryptfromMail(gossipMixin);

    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 1);
}

void AutocryptUtilsTest::test_report()
{
    // If content-type is multipart/report ignore message.

    MimeTreeParser::NodeHelper nodeHelper;
    {
        auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
        HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

        processAutocryptfromMail(mixin);
    }

    auto storage = AutocryptStorage::self();
    const auto alice = storage->getRecipient("alice@autocrypt.example");
    QVERIFY(alice);
    QCOMPARE(alice->count_have_ach(), 1);
    QCOMPARE(alice->count_no_ach(), 0);

    {
        auto message = readAndParseMail(u"autocrypt/mdn.mbox"_s);
        HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

        processAutocryptfromMail(mixin);
    }
    QCOMPARE(alice->count_have_ach(), 1);
    QCOMPARE(alice->count_no_ach(), 0);
}

void AutocryptUtilsTest::test_multiple_headers()
{
    // If there is more than one valid header, this SHOULD be treated as an
    // error, and all Autocrypt headers discarded as invalid.

    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/multiple_autocryptheaders.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

    processAutocryptfromMail(mixin);
    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 0);
}

void AutocryptUtilsTest::test_multiple_headers_invalid()
{
    // If there multiple autocrypt headers but only one is valid use this.
    //
    // An attribute name without a leading underscore is a “critical”
    // attribute.It MUST treat the entire Autocrypt header as invalid if it
    // encounters a “critical” attribute that it doesn’t support.

    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/multiple_autocryptheaders_invalid.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

    processAutocryptfromMail(mixin);
    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 1);
}

void AutocryptUtilsTest::test_multiple_headers_invalid_from()
{
    // If there multiple autocrypt headers but only one is valid use this.
    //
    // The addr attribute is mandatory, and contains the single recipient
    // address this header is valid for. If this address differs from the one
    // in the From header, the entire Autocrypt header MUST be treated as
    // invalid.

    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/multiple_autocryptheaders_invalid_from.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

    processAutocryptfromMail(mixin);
    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 1);
}

void AutocryptUtilsTest::test_multiple_from()
{
    // Messages SHOULD be ignored, if there is more than one address in the
    // From header.

    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/multiple_from.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

    processAutocryptfromMail(mixin);
    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 0);
}

void AutocryptUtilsTest::test_non_autocrypt()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

    processAutocryptfromMail(mixin);
    auto storage = AutocryptStorage::self();
    QCOMPARE(storage->d_func()->recipients.size(), 0);
}

void AutocryptUtilsTest::test_update_autocrypt()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.get());
    auto messageDate = QDateTime::currentDateTime().addYears(-1);

    message->date()->setDateTime(messageDate);
    processAutocryptfromMail(mixin);

    auto newDate = messageDate.addDays(2);
    message->date()->setDateTime(newDate);
    processAutocryptfromMail(mixin);

    auto storage = AutocryptStorage::self();
    const auto alice = storage->getRecipient("alice@autocrypt.example");
    QVERIFY(alice);
    QCOMPARE(alice->count_have_ach(), 2);
    QCOMPARE(alice->autocrypt_timestamp(), newDate);
    QCOMPARE(alice->count_no_ach(), 0);
}

void AutocryptUtilsTest::test_update_non_autocrypt()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    auto newDate = messageDate.addDays(2);
    {
        auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
        HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

        message->date()->setDateTime(messageDate);
        processAutocryptfromMail(mixin);
    }

    {
        auto message = readAndParseMail(u"html.mbox"_s);
        HeaderMixupNodeHelper noAutocrypt(&nodeHelper, message.get());
        message->from()->from7BitString("alice@autocrypt.example");
        message->date()->setDateTime(newDate);
        processAutocryptfromMail(noAutocrypt);
    }

    auto storage = AutocryptStorage::self();
    const auto alice = storage->getRecipient("alice@autocrypt.example");
    QVERIFY(alice);
    QCOMPARE(alice->count_have_ach(), 1);
    QCOMPARE(alice->autocrypt_timestamp(), newDate);
    QCOMPARE(alice->count_no_ach(), 1);
}

void AutocryptUtilsTest::test_update_autocrypt_gossip()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    auto newDate = messageDate.addDays(2);
    {
        auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
        HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

        message->date()->setDateTime(messageDate);
        processAutocryptfromMail(mixin);
    }

    {
        auto message = readAndParseMail(u"autocrypt/alice_gossip.mbox"_s);
        HeaderMixupNodeHelper mixin(&nodeHelper, message.get());

        message->date()->setDateTime(newDate);

        MimeTreeParser::SimpleObjectTreeSource testSource;
        MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
        testSource.setDecryptMessage(true);
        otp.parseObjectTree(message.get());

        auto extraContent = nodeHelper.decryptedNodeForContent(message.get());
        QVERIFY(extraContent);
        auto dateHeader = static_cast<KMime::Headers::Date *>(extraContent->headerByType("date"));
        dateHeader->setDateTime(newDate);

        processAutocryptfromMail(mixin);
    }

    auto storage = AutocryptStorage::self();
    const auto alice = storage->getRecipient("alice@autocrypt.example");
    QVERIFY(alice);
    QCOMPARE(alice->count_have_ach(), 1);
    QCOMPARE(alice->autocrypt_timestamp(), messageDate);
    QCOMPARE(alice->count_no_ach(), 0);
    QCOMPARE(alice->gossip_timestamp(), newDate);
}

#include "moc_autocryptutilstest.cpp"
