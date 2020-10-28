/* SPDX-FileCopyrightText: 2020 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "autocryptrecipienttest.h"

#include <autocrypt/autocryptrecipient.h>

#include <gpgme++/data.h>

#include <QGpgME/DataProvider>
#include <QGpgME/Protocol>

#include "qtest.h"
#include <QUrl>
#include <QStandardPaths>
#include <QJsonObject>

#include <QFile>
#include <kcodecs.h>

KMime::Message::Ptr readAndParseMail(const QString &mailFile)
{
    QFile file(QLatin1String(MAIL_DATA_DIR) + QLatin1Char('/') + mailFile);
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
    qputenv("LC_ALL", "C");
    qputenv("KDEHOME", QFile::encodeName(QDir::homePath() + QLatin1String("/.qttest")).constData());
    QStandardPaths::setTestModeEnabled(true);
}


void AutocryptRecipientTest::test_defaults()
{
    auto recipient = AutocryptRecipient();

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(QStringLiteral("addr")).toString(), QStringLiteral());
    QCOMPARE(obj.value(QStringLiteral("count_have_ach")).toInt(), 0);
    QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 0);
    QCOMPARE(obj.value(QStringLiteral("prefer_encrypt")).toBool(), false);
    QCOMPARE(obj.value(QStringLiteral("keydata")).toString(), QStringLiteral());
    QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), QStringLiteral());

    QCOMPARE(obj.contains(QStringLiteral("last_seen")), false);
    QCOMPARE(obj.contains(QStringLiteral("counting_since")), false);
    QCOMPARE(obj.contains(QStringLiteral("bad_user_agent")), false);
}

void AutocryptRecipientTest::test_firstAutocryptHeader()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(QStringLiteral("autocrypt/header.mbox"));
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin);

    auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
    QVERIFY(document.isObject());
    const auto &obj = document.object();

    QCOMPARE(obj.value(QStringLiteral("addr")).toString(), QStringLiteral("alice@autocrypt.example"));
    QCOMPARE(obj.value(QStringLiteral("count_have_ach")).toInt(), 1);
    QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 0);
    QCOMPARE(obj.value(QStringLiteral("prefer_encrypt")).toBool(), true);
    QCOMPARE(obj.value(QStringLiteral("keydata")).toString(), QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFgIDAQIeAQIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
    QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), QStringLiteral("2019-01-22T12:56:25+01:00"));
    QCOMPARE(obj.value(QStringLiteral("last_seen")).toString(), QStringLiteral("2019-01-22T12:56:25+01:00"));
    QCOMPARE(obj.value(QStringLiteral("counting_since")).toString(), QStringLiteral("2019-01-22T12:56:25+01:00"));

    QCOMPARE(obj.contains(QStringLiteral("bad_user_agent")), false);
}

void AutocryptRecipientTest::test_initiateWithNoAutocryptHeader()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(QStringLiteral("html.mbox"));
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin);

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("addr")).toString(), QStringLiteral(""));
        QCOMPARE(obj.value(QStringLiteral("count_have_ach")).toInt(), 0);
        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 1);
        QCOMPARE(obj.value(QStringLiteral("prefer_encrypt")).toBool(), false);
        QCOMPARE(obj.value(QStringLiteral("keydata")).toString(), QStringLiteral(""));
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), QStringLiteral("2011-05-26T01:16:54+01:00"));
        QCOMPARE(obj.value(QStringLiteral("counting_since")).toString(), QStringLiteral("2011-05-26T01:16:54+01:00"));
        QCOMPARE(obj.contains(QStringLiteral("last_seen")), false);
        QCOMPARE(obj.value(QStringLiteral("bad_user_agent")).toString(), QStringLiteral("KMail/4.6 beta5 (Linux/2.6.34.7-0.7-desktop; KDE/4.6.41; x86_64; git-0269848; 2011-04-19)"));
    }

    message = readAndParseMail(QStringLiteral("autocrypt/header.mbox"));
    mixin.message = message.data();
    recipient.updateFromMessage(mixin);

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("addr")).toString(), QStringLiteral("alice@autocrypt.example"));
        QCOMPARE(obj.value(QStringLiteral("count_have_ach")).toInt(), 1);
        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 1);
        QCOMPARE(obj.value(QStringLiteral("prefer_encrypt")).toBool(), true);
        QCOMPARE(obj.value(QStringLiteral("keydata")).toString(), QStringLiteral("mDMEXEcE6RYJKwYBBAHaRw8BAQdArjWwk3FAqyiFbFBKT4TzXcVBqPTB3gmzlC/Ub7O1u120F2FsaWNlQGF1dG9jcnlwdC5leGFtcGxliJYEExYIAD4WIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbAwUJA8JnAAULCQgHAgYVCgkICwIEFgIDAQIeAQIXgAAKCRDyMVUMT0fjjkqLAP9frlijwBJvA+HFnqCZcYIVxlyXzS5Gi5gMTpp37K73jgD/VbKYhkwk9iu689OYH4K7q7LbmdeaJ+RX88Y/ad9hZwy4OARcRwTpEgorBgEEAZdVAQUBAQdAQv8GIa2rSTzgqbXCpDDYMiKRVitCsy203x3sE9+eviIDAQgHiHgEGBYIACAWIQTrhbtfozp14V6UTmPyMVUMT0fjjgUCXEcE6QIbDAAKCRDyMVUMT0fjjlnQAQDFHUs6TIcxrNTtEZFjUFm1M0PJ1Dng/cDW4xN80fsn0QEA22Kr7VkCjeAEC08VSTeV+QFsmz55/lntWkwYWhmvOgE"));
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), QStringLiteral("2019-01-22T12:56:25+01:00"));
        QCOMPARE(obj.value(QStringLiteral("counting_since")).toString(), QStringLiteral("2011-05-26T01:16:54+01:00"));
        QCOMPARE(obj.value(QStringLiteral("last_seen")).toString(), QStringLiteral("2019-01-22T12:56:25+01:00"));
        QCOMPARE(obj.value(QStringLiteral("bad_user_agent")).toString(), QStringLiteral("KMail/4.6 beta5 (Linux/2.6.34.7-0.7-desktop; KDE/4.6.41; x86_64; git-0269848; 2011-04-19)"));
    }
}

void AutocryptRecipientTest::test_coreUpdateLogic()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(QStringLiteral("html.mbox"));
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    // Set a date header, that we know to pass the update logic
    auto messageDate = QDateTime::currentDateTime().addYears(-1);
    message->date()->setDateTime(messageDate);

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin);

    {
        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 1);
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), messageDate.toString(Qt::ISODate));
        QCOMPARE(obj.value(QStringLiteral("counting_since")).toString(), messageDate.toString(Qt::ISODate));
    }

    {   // Do not update when passing the same message a second time
        recipient.updateFromMessage(mixin);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 1);
    }

    {   // Do not update, if the header date is in the future
        auto newDate = QDateTime::currentDateTime().addDays(2);
        message->date()->setDateTime(newDate);

        recipient.updateFromMessage(mixin);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 1);
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), messageDate.toString(Qt::ISODate));
    }


    {   // Update the timestamp and count, if we have a new mail
        auto newDate = QDateTime::currentDateTime().addDays(-2);
        message->date()->setDateTime(newDate);
        messageDate = newDate;
        recipient.updateFromMessage(mixin);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 2);
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), newDate.toString(Qt::ISODate));
    }

    {   // Do not update when parsing an older mail
        message->date()->setDateTime(messageDate.addDays(-1));
        recipient.updateFromMessage(mixin);

        auto document = QJsonDocument::fromJson(recipient.toJson(QJsonDocument::Compact));
        QVERIFY(document.isObject());
        const auto &obj = document.object();

        QCOMPARE(obj.value(QStringLiteral("count_no_ach")).toInt(), 2);
        QCOMPARE(obj.value(QStringLiteral("autocrypt_timestamp")).toString(), messageDate.toString(Qt::ISODate));
    }
}
void AutocryptRecipientTest::test_gpgKey()
{
    MimeTreeParser::NodeHelper nodeHelper;
    auto message = readAndParseMail(QStringLiteral("autocrypt/header.mbox"));
    HeaderMixupNodeHelper mixin(&nodeHelper, message.data());

    auto recipient = AutocryptRecipient();
    recipient.updateFromMessage(mixin);

    const auto &key = recipient.gpgKey();

    QCOMPARE(key.primaryFingerprint(), "EB85BB5FA33A75E15E944E63F231550C4F47E38E");
    QCOMPARE(key.userID(0).email(), "alice@autocrypt.example");
}

