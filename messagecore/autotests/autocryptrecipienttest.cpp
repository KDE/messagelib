/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "autocryptrecipienttest.h"
using namespace Qt::Literals::StringLiterals;

#include "autocrypt/autocryptrecipient.h"
#include "autocrypt/autocryptrecipient_p.h"
#include "autocrypt/autocryptutils.h"

#include <MimeTreeParser/ObjectTreeParser>
#include <MimeTreeParser/SimpleObjectTreeSource>

#include <KCodecs>

#include <gpgme++/data.h>

#include <QGpgME/DataProvider>
#include <QGpgME/Protocol>

#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTest>

static KMime::Message::Ptr readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1StringView(MAIL_DATA_DIR) + u'/' + mailFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = KMime::CRLFtoLF(file.readAll());
    Q_ASSERT(!data.isEmpty());
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(data);
    msg->parse();
    return msg;
}

using namespace MessageCore;

QTEST_MAIN(AutocryptRecipientTest)

void AutocryptRecipientTest::initTestCase()
{
    qputenv("LC_ALL", "en_US.UTF-8");
    QStandardPaths::setTestModeEnabled(true);
}

void AutocryptRecipientTest::test_defaults()
{
    auto recipient = AutocryptRecipient();

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(u"addr"_s).toString(), QStringLiteral());
    QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), false);
    QCOMPARE(obj.value(u"keydata"_s).toString(), QStringLiteral());
    QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), QStringLiteral());

    QCOMPARE(obj.contains(u"last_seen"_s), false);
    QCOMPARE(obj.contains(u"counting_since"_s), false);
    QCOMPARE(obj.contains(u"bad_user_agent"_s), false);

    QCOMPARE(obj.contains(u"gossip_timestamp"_s), false);
    QCOMPARE(obj.contains(u"gossip_key"_s), false);
}

void AutocryptRecipientTest::test_firstAutocryptHeader()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(u"addr"_s).toString(), u"alice@autocrypt.example"_s);
    QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 1);
    QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), true);
    QCOMPARE(obj.value(u"keydata"_s).toString(),
             QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/"
                            "Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFgIDAQIeA"
                            "QIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/"
                            "ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+"
                            "eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/"
                            "cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
    QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
    QCOMPARE(obj.value(u"last_seen"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
    QCOMPARE(obj.value(u"counting_since"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);

    QCOMPARE(obj.contains(u"bad_user_agent"_s), false);
}

void AutocryptRecipientTest::test_fromJson()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto other = AutocryptRecipient();
    other.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    auto recipient = AutocryptRecipient();
    recipient.fromJson(other.toJson(QJsonDocument::Compact));

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(u"addr"_s).toString(), u"alice@autocrypt.example"_s);
    QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 1);
    QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), true);
    QCOMPARE(obj.value(u"keydata"_s).toString(),
             QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/"
                            "Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFgIDAQIeA"
                            "QIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/"
                            "ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+"
                            "eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/"
                            "cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
    QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
    QCOMPARE(obj.value(u"last_seen"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
    QCOMPARE(obj.value(u"counting_since"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);

    QCOMPARE(obj.contains(u"bad_user_agent"_s), false);
}

void AutocryptRecipientTest::test_fromJsonGossip()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    KMime::Headers::Generic gossipHeader("Autocrypt-Gossip");
    QByteArray keydata(
        "mDMEXEcE6RYJKwYBBAHaRw8BAQdAPPy13Q7Y8w2VPRkksrijrn9o8u59ra1c2CJiHFpbM2G0FWJvYkBhdXRvY3J5cHQuZXhhbXBsZYiWBBMWCAA+"
        "FiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwMFCQPCZwAFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AACgkQ4w5v3UWQH4IfwAEA3lujohz3Nj9afUnaGUXN7YboIzQsmpgGkN8thyb/"
        "slIBAKwdJyg1SurKqHnxy3Wl/DBzOrR12/"
        "pN7nScn0+"
        "x4sgBuDgEXEcE6RIKKwYBBAGXVQEFAQEHQJSU7QErtJOYXsIagw2qwnVbt31ooVEx8Xcb476NCbFjAwEIB4h4BBgWCAAgFiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwwACgkQ4w5v3UWQH"
        "4LlHQEAlwUBfUU8ORC0RAS/dzlZSEm7+ImY12Wv8QGUCx5zPbUA/3YH84ZOAQDbmV/C+R//0WVNbGfav9X5KYmiratYR7oL");

    gossipHeader.from7BitString(QByteArray("addr=bob@autocrypt.example; keydata=\n" + keydata));
    auto other = AutocryptRecipient();
    other.updateFromGossip(mixin, &gossipHeader);

    auto recipient = AutocryptRecipient();
    recipient.fromJson(other.toJson(QJsonDocument::Compact));

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(u"addr"_s).toString(), u"bob@autocrypt.example"_s);
    QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), false);
    QCOMPARE(obj.value(u"keydata"_s).toString(), QStringLiteral());
    QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), QStringLiteral());

    QCOMPARE(obj.contains(u"last_seen"_s), false);
    QCOMPARE(obj.contains(u"counting_since"_s), false);
    QCOMPARE(obj.contains(u"bad_user_agent"_s), false);

    QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), message->date()->dateTime().toString(Qt::ISODate));
    QCOMPARE(obj.value(u"gossip_key"_s).toString(), QString::fromLatin1(keydata));
}

void AutocryptRecipientTest::test_initiateWithNoAutocryptHeader()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"addr"_s).toString(), u""_s);
        QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 0);
        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 1);
        QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), false);
        QCOMPARE(obj.value(u"keydata"_s).toString(), u""_s);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), u"2011-05-26T01:16:54+01:00"_s);
        QCOMPARE(obj.value(u"counting_since"_s).toString(), u"2011-05-26T01:16:54+01:00"_s);
        QCOMPARE(obj.contains(u"last_seen"_s), false);
        QCOMPARE(obj.value(u"bad_user_agent"_s).toString(), u"KMail/4.6 beta5 (Linux/2.6.34.7-0.7-desktop; KDE/4.6.41; x86_64; git-0269848; 2011-04-19)"_s);
    }

    message = readAndParseMail(u"autocrypt/header.mbox"_s);
    mixin.message = message.data();
    recipient.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"addr"_s).toString(), u"alice@autocrypt.example"_s);
        QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 1);
        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 1);
        QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), true);
        QCOMPARE(obj.value(u"keydata"_s).toString(),
                 QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/"
                                "Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFgIDA"
                                "QIeAQIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/"
                                "ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+"
                                "eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/"
                                "cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
        QCOMPARE(obj.value(u"counting_since"_s).toString(), u"2011-05-26T01:16:54+01:00"_s);
        QCOMPARE(obj.value(u"last_seen"_s).toString(), u"2019-01-22T12:56:25+01:00"_s);
        QCOMPARE(obj.value(u"bad_user_agent"_s).toString(), u"KMail/4.6 beta5 (Linux/2.6.34.7-0.7-desktop; KDE/4.6.41; x86_64; git-0269848; 2011-04-19)"_s);
    }
}

void AutocryptRecipientTest::test_coreUpdateLogic()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    // Set a date header, that we know to pass the update logic
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    message->date()->setDateTime(messageDate);

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin, nullptr);

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 1);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
        QCOMPARE(obj.value(u"counting_since"_s).toString(), messageDate.toString(Qt::ISODate));
    }

    { // Do not update when passing the same message a second time
        recipient.updateFromMessage(mixin, nullptr);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 1);
    }

    { // Do not update, if the header date is in the future
        auto newDate = QDateTime::currentDateTime().addDays(2);
        message->date()->setDateTime(newDate);

        recipient.updateFromMessage(mixin, nullptr);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 1);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
    }

    { // Update the timestamp and count, if we have a new mail
        auto newDate = QDateTime::currentDateTime().addDays(-2);
        message->date()->setDateTime(newDate);
        messageDate = newDate;
        recipient.updateFromMessage(mixin, nullptr);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 2);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), newDate.toString(Qt::ISODate));
    }

    { // Do not update when parsing an older mail
        message->date()->setDateTime(messageDate.addDays(-1));
        recipient.updateFromMessage(mixin, nullptr);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 2);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
    }
}

void AutocryptRecipientTest::test_changedLogic()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    // Set a date header, that we know to pass the update logic
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    message->date()->setDateTime(messageDate);

    auto recipient = AutocryptRecipient();
    QCOMPARE(recipient.hasChanged(), false);

    recipient.updateFromMessage(mixin, nullptr);
    QCOMPARE(recipient.hasChanged(), true);

    recipient.setChangedFlag(false);
    QCOMPARE(recipient.hasChanged(), false);

    { // Do not update when passing the same message a second time
        recipient.updateFromMessage(mixin, nullptr);
        QCOMPARE(recipient.hasChanged(), false);
    }

    { // Do not update, if the header date is in the future
        auto newDate = QDateTime::currentDateTime().addDays(2);
        message->date()->setDateTime(newDate);

        recipient.updateFromMessage(mixin, nullptr);
        QCOMPARE(recipient.hasChanged(), false);
    }

    { // Update the timestamp and count, if we have a new mail
        auto newDate = QDateTime::currentDateTime().addDays(-2);
        message->date()->setDateTime(newDate);
        messageDate = newDate;
        recipient.updateFromMessage(mixin, nullptr);

        QCOMPARE(recipient.hasChanged(), true);

        recipient.setChangedFlag(false);
        QCOMPARE(recipient.hasChanged(), false);
    }

    { // Do not update when parsing an older mail
        message->date()->setDateTime(messageDate.addDays(-1));
        recipient.updateFromMessage(mixin, nullptr);
        QCOMPARE(recipient.hasChanged(), false);
    }
}

void AutocryptRecipientTest::test_gpgKey()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    const auto &key = recipient.gpgKey();

    QCOMPARE(key.primaryFingerprint(), "EB85BB5FA33A75E15E944E63F231550C4F47E38E");
    QCOMPARE(key.userID(0).email(), "alice@autocrypt.example");
}

void AutocryptRecipientTest::test_setGossipHeader()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto recipient = AutocryptRecipient();
    auto message = readAndParseMail(u"autocrypt/gossipheader.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.data());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.data());

    auto gossipHeader = gossipMixin.mailHeaderAsBase("Autocrypt-Gossip");
    QVERIFY(gossipHeader);
    recipient.updateFromGossip(gossipMixin, gossipHeader);

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(u"addr"_s).toString(), u"bob@autocrypt.example"_s);
    QCOMPARE(obj.value(u"count_have_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
    QCOMPARE(obj.value(u"prefer_encrypt"_s).toBool(), false);
    QCOMPARE(obj.value(u"keydata"_s).toString(), QStringLiteral());
    QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), QStringLiteral());

    QCOMPARE(obj.contains(u"last_seen"_s), false);
    QCOMPARE(obj.contains(u"counting_since"_s), false);
    QCOMPARE(obj.contains(u"bad_user_agent"_s), false);

    QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), u"2019-01-22T12:56:29+01:00"_s);
    QCOMPARE(obj.value(u"gossip_key"_s).toString(),
             QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdAPPy13Q7Y8w2VPRkksrijrn9o8u59ra1c2CJiHFpbM2G0FWJvYkBhdXRvY3J5cHQuZXhhbXBsZYiWBBMWCAA+"
                            "FiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwMFCQPCZwAFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AACgkQ4w5v3UWQH4IfwAEA3lujohz3Nj9afUnaGUXN7YboIz"
                            "QsmpgGkN8thyb/slIBAKwdJyg1SurKqHnxy3Wl/DBzOrR12/"
                            "pN7nScn0+"
                            "x4sgBuDgEXEcE6RIKKwYBBAGXVQEFAQEHQJSU7QErtJOYXsIagw2qwnVbt31ooVEx8Xcb476NCbFjAwEIB4h4BBgWCAAgFiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHB"
                            "OkCGwwACgkQ4w5v3UWQH4LlHQEAlwUBfUU8ORC0RAS/dzlZSEm7+ImY12Wv8QGUCx5zPbUA/3YH84ZOAQDbmV/C+R//0WVNbGfav9X5KYmiratYR7oL"));
}

void AutocryptRecipientTest::test_gossipUpdateLogic()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    KMime::Headers::Generic gossipHeader("Autocrypt-Gossip");
    KMime::Headers::Generic gossipHeader2("Autocrypt-Gossip");

    QByteArray keydata(
        "mDMEXEcE6RYJKwYBBAHaRw8BAQdAPPy13Q7Y8w2VPRkksrijrn9o8u59ra1c2CJiHFpbM2G0FWJvYkBhdXRvY3J5cHQuZXhhbXBsZYiWBBMWCAA+"
        "FiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwMFCQPCZwAFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AACgkQ4w5v3UWQH4IfwAEA3lujohz3Nj9afUnaGUXN7YboIzQsmpgGkN8thyb/"
        "slIBAKwdJyg1SurKqHnxy3Wl/DBzOrR12/"
        "pN7nScn0+"
        "x4sgBuDgEXEcE6RIKKwYBBAGXVQEFAQEHQJSU7QErtJOYXsIagw2qwnVbt31ooVEx8Xcb476NCbFjAwEIB4h4BBgWCAAgFiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwwACgkQ4w5v3UWQH"
        "4LlHQEAlwUBfUU8ORC0RAS/dzlZSEm7+ImY12Wv8QGUCx5zPbUA/3YH84ZOAQDbmV/C+R//0WVNbGfav9X5KYmiratYR7oL");

    QByteArray keydata2 = "second";

    gossipHeader.from7BitString(QByteArray("addr=bob@autocrypt.example; keydata=\n" + keydata));
    gossipHeader2.from7BitString(QByteArray("addr=bob@autocrypt.example; keydata=\n" + keydata2));

    // Set a date header, that we know to pass the update logic
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    message->date()->setDateTime(messageDate);

    auto recipient = AutocryptRecipient();
    recipient.updateFromGossip(mixin, &gossipHeader);

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"count_no_ach"_s).toInt(), 0);
        QCOMPARE(obj.value(u"autocrypt_timestamp"_s).toString(), QStringLiteral());
        QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
    }

    { // Do not update when passing the same message a second time
        // To verify that nothing is updated, parse a different header
        // and test if gossip_key was not updated
        recipient.updateFromGossip(mixin, &gossipHeader2);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"gossip_key"_s).toString(), QString::fromLatin1(keydata));
    }

    { // Do not update, if the header date is in the future
        // To verify that nothing is updated, parse a different header
        // and test if gossip_key was not updated
        auto newDate = QDateTime::currentDateTime().addDays(2);
        message->date()->setDateTime(newDate);

        recipient.updateFromGossip(mixin, &gossipHeader2);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
        QCOMPARE(obj.value(u"gossip_key"_s).toString(), QString::fromLatin1(keydata));
    }

    { // Update the timestamp and count, if we have a new mail
        // To verify that gossip is updated, parse a different header
        // with different keydata

        auto newDate = QDateTime::currentDateTime().addDays(-2);
        message->date()->setDateTime(newDate);
        messageDate = newDate;
        recipient.updateFromGossip(mixin, &gossipHeader2);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
        QCOMPARE(obj.value(u"gossip_key"_s).toString(), QString::fromLatin1(keydata2));
    }

    { // Do not update when parsing an older mail
        // To verify that nothing is updated, parse a different header
        // and test if gossip_key was not updated
        message->date()->setDateTime(messageDate.addDays(-1));
        recipient.updateFromGossip(mixin, &gossipHeader);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(u"gossip_timestamp"_s).toString(), messageDate.toString(Qt::ISODate));
        QCOMPARE(obj.value(u"gossip_key"_s).toString(), QString::fromLatin1(keydata2));
    }
}

void AutocryptRecipientTest::test_changedLogicGossip()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"html.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    KMime::Headers::Generic gossipHeader("Autocrypt-Gossip");

    QByteArray keydata(
        "mDMEXEcE6RYJKwYBBAHaRw8BAQdAPPy13Q7Y8w2VPRkksrijrn9o8u59ra1c2CJiHFpbM2G0FWJvYkBhdXRvY3J5cHQuZXhhbXBsZYiWBBMWCAA+"
        "FiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwMFCQPCZwAFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AACgkQ4w5v3UWQH4IfwAEA3lujohz3Nj9afUnaGUXN7YboIzQsmpgGkN8thyb/"
        "slIBAKwdJyg1SurKqHnxy3Wl/DBzOrR12/"
        "pN7nScn0+"
        "x4sgBuDgEXEcE6RIKKwYBBAGXVQEFAQEHQJSU7QErtJOYXsIagw2qwnVbt31ooVEx8Xcb476NCbFjAwEIB4h4BBgWCAAgFiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwwACgkQ4w5v3UWQH"
        "4LlHQEAlwUBfUU8ORC0RAS/dzlZSEm7+ImY12Wv8QGUCx5zPbUA/3YH84ZOAQDbmV/C+R//0WVNbGfav9X5KYmiratYR7oL");

    gossipHeader.from7BitString(QByteArray("addr=bob@autocrypt.example; keydata=\n" + keydata));

    // Set a date header, that we know to pass the update logic
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    message->date()->setDateTime(messageDate);

    auto recipient = AutocryptRecipient();
    QCOMPARE(recipient.hasChanged(), false);

    recipient.updateFromGossip(mixin, &gossipHeader);

    QCOMPARE(recipient.hasChanged(), true);
    recipient.setChangedFlag(false);
    QCOMPARE(recipient.hasChanged(), false);

    // Do not update when passing the same message a second time
    recipient.updateFromGossip(mixin, &gossipHeader);
    QCOMPARE(recipient.hasChanged(), false);

    { // Do not update, if the header date is in the future
        auto newDate = QDateTime::currentDateTime().addDays(2);
        message->date()->setDateTime(newDate);

        recipient.updateFromGossip(mixin, &gossipHeader);
        QCOMPARE(recipient.hasChanged(), false);
    }

    { // Update the timestamp and count, if we have a new mail

        auto newDate = QDateTime::currentDateTime().addDays(-2);
        message->date()->setDateTime(newDate);
        messageDate = newDate;
        recipient.updateFromGossip(mixin, &gossipHeader);
        QCOMPARE(recipient.hasChanged(), true);

        recipient.setChangedFlag(false);
        QCOMPARE(recipient.hasChanged(), false);
    }

    // Do not update when parsing an older mail
    message->date()->setDateTime(messageDate.addDays(-1));
    recipient.updateFromGossip(mixin, &gossipHeader);

    QCOMPARE(recipient.hasChanged(), false);
}

void AutocryptRecipientTest::test_gossipKey()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto recipient = AutocryptRecipient();
    auto message = readAndParseMail(u"autocrypt/gossipheader.mbox"_s);
    HeaderMixupNodeHelper gossipMixin(&nodeHelper, message.data());

    MimeTreeParser::SimpleObjectTreeSource testSource;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    testSource.setDecryptMessage(true);
    otp.parseObjectTree(message.data());

    QCOMPARE(recipient.gossipKey().isNull(), true);

    auto gossipHeader = gossipMixin.mailHeaderAsBase("Autocrypt-Gossip");
    QVERIFY(gossipHeader);
    recipient.updateFromGossip(gossipMixin, gossipHeader);

    const auto &gossipKey = recipient.gossipKey();
    QCOMPARE(gossipKey.isNull(), false);
    QCOMPARE(gossipKey.primaryFingerprint(), "F0541EA82D3100AA1ADF3B1EE30E6FDD45901F82");
    QCOMPARE(gossipKey.userID(0).email(), "bob@autocrypt.example");

    QCOMPARE(
        recipient.gossipKeydata(),
        KCodecs::base64Decode("mDMEXEcE6RYJKwYBBAHaRw8BAQdAPPy13Q7Y8w2VPRkksrijrn9o8u59ra1c2CJiHFpbM2G0FWJvYkBhdXRvY3J5cHQuZXhhbXBsZYiWBBMWCAA+"
                              "FiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlxHBOkCGwMFCQPCZwAFCwkIBwIGFQoJCAsCBBYCAwECHgECF4AACgkQ4w5v3UWQH4IfwAEA3lujohz3Nj9afUnaGUXN7Ybo"
                              "IzQsmpgGkN8thyb/slIBAKwdJyg1SurKqHnxy3Wl/DBzOrR12/"
                              "pN7nScn0+"
                              "x4sgBuDgEXEcE6RIKKwYBBAGXVQEFAQEHQJSU7QErtJOYXsIagw2qwnVbt31ooVEx8Xcb476NCbFjAwEIB4h4BBgWCAAgFiEE8FQeqC0xAKoa3zse4w5v3UWQH4IFAlx"
                              "HBOkCGwwACgkQ4w5v3UWQH4LlHQEAlwUBfUU8ORC0RAS/dzlZSEm7+ImY12Wv8QGUCx5zPbUA/3YH84ZOAQDbmV/C+R//0WVNbGfav9X5KYmiratYR7oL"));
}

void AutocryptRecipientTest::test_getters()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(u"autocrypt/header.mbox"_s);
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin, mixin.mailHeaderAsBase("Autocrypt"));

    QCOMPARE(recipient.addr(), "alice@autocrypt.example");
    QCOMPARE(recipient.count_have_ach(), 1);
    QCOMPARE(recipient.count_no_ach(), 0);
    QCOMPARE(recipient.prefer_encrypt(), true);
    QCOMPARE(recipient.gpgKeydata(),
             KCodecs::base64Decode("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/"
                                   "Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFg"
                                   "IDAQIeAQIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/"
                                   "ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+"
                                   "eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/"
                                   "cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
    QCOMPARE(recipient.autocrypt_timestamp(), QDateTime::fromString(u"2019-01-22T12:56:25+01:00"_s, Qt::ISODate));
    QCOMPARE(recipient.last_seen(), QDateTime::fromString(u"2019-01-22T12:56:25+01:00"_s, Qt::ISODate));
    QCOMPARE(recipient.counting_since(), QDateTime::fromString(u"2019-01-22T12:56:25+01:00"_s, Qt::ISODate));

    QCOMPARE(recipient.bad_user_agent(), QByteArray());
}

#include "moc_autocryptrecipienttest.cpp"
