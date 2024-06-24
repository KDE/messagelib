/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messagefactoryngtest.h"

#include "cryptofunctions.h"
#include "qtest_messagecomposer.h"
#include "setupenv.h"

#include <MessageCore/StringUtil>

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MessageComposerSettings>
#include <MessageComposer/MessageFactoryNG>
#include <MessageComposer/Util>

#include <MessageComposer/InfoPart>
#include <MessageComposer/TextPart>

#include <MessageCore/DateFormatter>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>

#include "globalsettings_templateparser.h"

#include <QDateTime>
#include <QDir>
#include <QLocale>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSignalSpy>
#include <QTest>

using namespace MessageComposer;

MessageFactoryTest::MessageFactoryTest(QObject *parent)
    : QObject(parent)
{
}

MessageFactoryTest::~MessageFactoryTest()
{
    // Workaround QTestLib not flushing deleteLater()s on exit, which
    // leads to WebEngine asserts (view not deleted)
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

void MessageFactoryTest::initMain()
{
#ifndef Q_OS_WIN
    Test::setupFullEnv();
#else
    Test::setupEnv();
#endif
}

namespace
{
template<typename String>
String very_simplistic_diff(const String &a, const String &b)
{
    const QList<String> al = a.split('\n');
    const QList<String> bl = b.split('\n');
    String result;
    int ai = 0;
    int bi = 0;
    while (ai < al.size() && bi < bl.size()) {
        if (al[ai] == bl[bi]) {
            // qDebug( "found   equal line a@%d x b@%d", ai, bi );
            result += "  " + al[ai] + '\n';
            ++ai;
            ++bi;
        } else {
            // qDebug( "found unequal line a@%d x b@%d", ai, bi );
            const int b_in_a = al.indexOf(bl[bi], ai);
            const int a_in_b = bl.indexOf(al[ai], bi);
            // qDebug( "   b_in_a == %d", b_in_a );
            // qDebug( "   a_in_b == %d", a_in_b );
            if (b_in_a == -1) {
                if (a_in_b == -1) {
                    // (at least) one line changed:
                    result += "- " + al[ai++] + '\n' + "+ " + bl[bi++] + '\n';
                } else {
                    // some lines added:
                    while (bi < a_in_b) {
                        result += "+ " + bl[bi++] + '\n';
                    }
                }
            } else {
                // some lines removed:
                while (ai < b_in_a) {
                    result += "- " + al[ai++] + '\n';
                }
                // some lines added:
                while (bi < a_in_b) {
                    result += "+ " + bl[bi++] + '\n';
                }
            }
            // qDebug( "result ( a@%d b@%d ):\n%s\n--end", ai, bi, result.constData() );
        }
    }

    const int sizeal(al.size());
    for (int i = ai; i < sizeal; ++i) {
        result += "- " + al[i] + '\n';
    }
    const int sizebl(bl.size());
    for (int i = bi; i < sizebl; ++i) {
        result += "+ " + bl[i] + '\n';
    }
    return result;
}
}

#define QCOMPARE_OR_DIFF(a, b)                                                                                                                                 \
    if (a != b) {                                                                                                                                              \
        qDebug("diff:\n--begin--\n%s\n--end--", very_simplistic_diff(a, b).constData());                                                                       \
    }                                                                                                                                                          \
    QVERIFY(a == b)

QTEST_MAIN(MessageFactoryTest)

void MessageFactoryTest::cleanupTestCase()
{
    delete mIdentMan;
    mIdentMan = nullptr;
    QDir dir(QDir::homePath() + QStringLiteral("/.qttest/"));
    dir.removeRecursively();
}

void MessageFactoryTest::initTestCase()
{
    qRegisterMetaType<MessageComposer::MessageFactoryNG::MessageReply>();
    mIdentMan = new KIdentityManagementCore::IdentityManager;

    KIdentityManagementCore::Identity &ident = mIdentMan->modifyIdentityForUoid(mIdentMan->defaultIdentity().uoid());
    ident.setFullName(QStringLiteral("another"));
    ident.setPrimaryEmailAddress(QStringLiteral("another@another.com"));

    mIdentMan->newFromScratch(QStringLiteral("test1"));
    mIdentMan->newFromScratch(QStringLiteral("test2"));
    mIdentMan->newFromScratch(QStringLiteral("test3"));
    mIdentMan->commit();
}

void MessageFactoryTest::init()
{
    TemplateParser::TemplateParserSettings::self()->setDefaults();
}

void MessageFactoryTest::testCreateReplyToAllWithUseSenderAndIdentityInCCAsync()
{
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));

    const QString filename(QStringLiteral(MAIL_DATA_DIR) + QStringLiteral("/replyall_with_identity_message_and_identity_in_cc.mbox"));
    KMime::Message::Ptr msg = Test::loadMessage(filename);
    KIdentityManagementCore::Identity &i1 = mIdentMan->modifyIdentityForName(QStringLiteral("test1"));
    i1.setFullName(QStringLiteral("foo1"));
    i1.setPrimaryEmailAddress(QStringLiteral("identity1@bla.com"));
    KIdentityManagementCore::Identity &i2 = mIdentMan->modifyIdentityForName(QStringLiteral("test2"));
    i2.setFullName(QStringLiteral("foo2"));
    i2.setPrimaryEmailAddress(QStringLiteral("identity2@bla.com"));
    mIdentMan->commit();

    MessageFactoryNG factory(msg, 0);
    factory.setReplyStrategy(ReplyAll);
    factory.setIdentityManager(mIdentMan);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();

    reply.replyAll = true;

    QString replyStr = QStringLiteral("> This is a mail for testing replyall and sender");
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Plain Message Test"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());

    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: foo1 <identity1@bla.com>\n"
                     "X-KMail-Identity: %1\n"
                     "Date: %2\n"
                     "Cc: blu <blu@blu.org>, bly <bly@bly.org>\n"
                     "To: blo <blo@blo.org>, bli <bli@bli.org>\n"
                     "Subject: Re: Plain Message Test\n"
                     "Content-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n"
                     "%3")
                     .arg(i1.uoid())
                     .arg(dateStr)
                     .arg(replyStr);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
}

void MessageFactoryTest::testCreateReplyToAllWithUseSenderAsync()
{
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));

    const QString filename(QStringLiteral(MAIL_DATA_DIR) + QStringLiteral("/replyall_with_identity_message.mbox"));
    KMime::Message::Ptr msg = Test::loadMessage(filename);
    KIdentityManagementCore::Identity &i1 = mIdentMan->modifyIdentityForName(QStringLiteral("test1"));
    i1.setFullName(QStringLiteral("foo1"));
    i1.setPrimaryEmailAddress(QStringLiteral("identity1@bla.com"));
    KIdentityManagementCore::Identity &i2 = mIdentMan->modifyIdentityForName(QStringLiteral("test2"));
    i2.setFullName(QStringLiteral("foo2"));
    i2.setPrimaryEmailAddress(QStringLiteral("identity2@bla.com"));
    mIdentMan->commit();

    MessageFactoryNG factory(msg, 0);
    factory.setReplyStrategy(ReplyAll);
    factory.setIdentityManager(mIdentMan);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QString replyStr = QStringLiteral("> This is a mail for testing replyall and sender");
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Plain Message Test"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());

    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: another <another@another.com>\n"
                     "Date: %1\n"
                     "Cc: blu <blu@blu.org>, bly <bly@bly.org>\n"
                     "To: blo <blo@blo.org>, bli <bli@bli.org>\n"
                     "Subject: Re: Plain Message Test\n"
                     "Content-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n"
                     "%2")
                     .arg(dateStr)
                     .arg(replyStr);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
}

void MessageFactoryTest::testCreateReplyToAllWithUseSenderByNoSameIdentitiesAsync()
{
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));

    const QString filename(QStringLiteral(MAIL_DATA_DIR) + QStringLiteral("/replyall_without_identity_message.mbox"));
    KMime::Message::Ptr msg = Test::loadMessage(filename);
    KIdentityManagementCore::Identity &i1 = mIdentMan->modifyIdentityForName(QStringLiteral("test1"));
    i1.setFullName(QStringLiteral("foo1"));
    i1.setPrimaryEmailAddress(QStringLiteral("identity1@bla.com"));
    KIdentityManagementCore::Identity &i2 = mIdentMan->modifyIdentityForName(QStringLiteral("test2"));
    i2.setFullName(QStringLiteral("foo2"));
    i2.setPrimaryEmailAddress(QStringLiteral("identity2@bla.com"));
    mIdentMan->commit();

    MessageFactoryNG factory(msg, 0);
    factory.setReplyStrategy(ReplyAll);
    factory.setIdentityManager(mIdentMan);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QString replyStr = QStringLiteral("> This is a mail for testing replyall and sender");
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Plain Message Test"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());

    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: another <another@another.com>\n"
                     "Date: %1\n"
                     "Cc: blu <blu@blu.org>, bly <bly@bly.org>\n"
                     "To: blo <blo@blo.org>, bli <bli@bli.org>, Bla <bloblo@bla.com>\n"
                     "Subject: Re: Plain Message Test\n"
                     "Content-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n"
                     "%2")
                     .arg(dateStr)
                     .arg(replyStr);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
}

void MessageFactoryTest::testCreateReplyToListAsync()
{
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));

    const QString filename(QStringLiteral(MAIL_DATA_DIR) + QStringLiteral("/list_message.mbox"));
    KMime::Message::Ptr msg = Test::loadMessage(filename);

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    factory.setReplyStrategy(ReplyList);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QDateTime date = msg->date()->dateTime();
    QString datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale::system().toString(date.time(), QLocale::LongFormat);
    QString replyStr = QString::fromLatin1(QByteArray("> This is a mail from ML"));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Plain Message Test"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());
    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: another <another@another.com>\n"
                     "Date: %1\n"
                     "To: list@list.org\n"
                     "Subject: Re: Plain Message Test\n"
                     "Content-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n"
                     "%2")
                     .arg(dateStr)
                     .arg(replyStr);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
}

void MessageFactoryTest::testCreateReplyToAuthorAsync()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    factory.setReplyStrategy(ReplyAuthor);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QDateTime date = msg->date()->dateTime();
    QString datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale::system().toString(date.time(), QLocale::LongFormat);
    QString replyStr = QString::fromUtf8(QByteArray(
        QByteArray("On ") + datetime.toUtf8() + QByteArray(" you wrote:\n> All happy families are alike; each unhappy family is unhappy in its own way.\n\n")));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Test Email Subject"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());
    QString replyTo = reply.msg->inReplyTo()->asUnicodeString();
    QString reference = reply.msg->references()->asUnicodeString();
    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: another <another@another.com>\n"
                     "Date: %1\n"
                     "X-KMail-Transport: 0\n"
                     "To: me@me.me\n"
                     "References: %3\n"
                     "In-Reply-To: %2\n"
                     "Subject: Re: Test Email Subject\n"
                     "X-KMail-CursorPos: %5\n"
                     "Content-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n"
                     "%4")
                     .arg(dateStr)
                     .arg(replyTo)
                     .arg(reference)
                     .arg(replyStr)
                     .arg(replyStr.length() - 1);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateReplyAllWithMultiEmailsAsync()
{
    KMime::Message::Ptr msg = createPlainTestMessageWithMultiEmails();

    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    factory.setReplyStrategy(ReplyAll);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Test Email Subject"));

    QString replyTo = reply.msg->inReplyTo()->asUnicodeString();
    QString reference = reply.msg->references()->asUnicodeString();
    QString dateStr = reply.msg->date()->asUnicodeString();
    QString ba = QString::fromLatin1(
                     "From: another <another@another.com>\n"
                     "Date: %1\n"
                     "X-KMail-Transport: 0\n"
                     "Cc: cc@cc.cc, cc2@cc.cc\n"
                     "To: you@you.you, you2@you.you, me@me.me\n"
                     "References: %3\n"
                     "In-Reply-To: %2\n"
                     "Subject: Re: Test Email Subject\nContent-Type: text/plain; charset=\"UTF-8\"\n"
                     "Content-Transfer-Encoding: 8Bit\nMIME-Version: 1.0\n"
                     "X-KMail-Link-Message: 0\n"
                     "X-KMail-Link-Type: reply\n\n> All happy families are alike; each unhappy family is unhappy in its own way.")
                     .arg(dateStr, replyTo, reference);
    QCOMPARE_OR_DIFF(reply.msg->encodedContent(), ba.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateReplyAllAsync()
{
    KMime::Message::Ptr msg = createPlainTestMessage();
    MessageFactoryNG factory(msg, 0);
    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);
    factory.setIdentityManager(mIdentMan);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QDateTime date = msg->date()->dateTime();
    QString datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale::system().toString(date.time(), QLocale::LongFormat);
    QString replyStr = QString::fromUtf8(QByteArray(
        QByteArray("On ") + datetime.toUtf8() + QByteArray(" you wrote:\n> All happy families are alike; each unhappy family is unhappy in its own way.\n\n")));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Test Email Subject"));
    QCOMPARE_OR_DIFF(reply.msg->body(), replyStr.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateReplyHtmlAsync()
{
    KMime::Message::Ptr msg = Test::loadMessageFromDataDir(QStringLiteral("html_utf8_encoded.mbox"));

    // qDebug() << "html message:" << msg->encodedContent();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    TemplateParser::TemplateParserSettings::self()->setReplyUsingVisualFormat(true);
    factory.setReplyAsHtml(true);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);
    factory.setIdentityManager(mIdentMan);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QDateTime date = msg->date()->dateTime().toLocalTime();
    QString datetime = QLocale().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale().toString(date.time(), QLocale::LongFormat);
    QString replyStr = QString::fromUtf8(QByteArray(QByteArray("On ") + datetime.toUtf8() + QByteArray(" you wrote:\n> encoded?\n\n")));
    QCOMPARE(reply.msg->contentType()->mimeType(), QByteArrayLiteral("multipart/alternative"));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: reply to please"));
    QCOMPARE(reply.msg->contents().count(), 2);
    QCOMPARE_OR_DIFF(reply.msg->contents().at(0)->body(), replyStr.toUtf8());

    TemplateParser::TemplateParserSettings::self()->setReplyUsingVisualFormat(false);
    factory.setReplyAsHtml(false);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 2);
    reply = spy.at(1).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();

    reply.replyAll = true;
    datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale::system().toString(date.time(), QLocale::LongFormat);
    QCOMPARE(reply.msg->contentType()->mimeType(), QByteArrayLiteral("text/plain"));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: reply to please"));
    QCOMPARE(reply.msg->contents().count(), 0);
    msg.clear();
}

void MessageFactoryTest::testCreateReplyUTF16Base64Async()
{
    KMime::Message::Ptr msg = Test::loadMessageFromDataDir(QStringLiteral("plain_utf16.mbox"));
    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);

    TemplateParser::TemplateParserSettings::self()->setReplyUsingVisualFormat(true);
    factory.setReplyAsHtml(true);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);
    factory.setIdentityManager(mIdentMan);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;

    QDateTime date = msg->date()->dateTime().toLocalTime();
    QString datetime = QLocale().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1Char(' ') + QLocale().toString(date.time(), QLocale::LongFormat);
    QString replyStr = QString::fromUtf8(QByteArray(QByteArray("On ") + datetime.toUtf8() + QByteArray(" you wrote:\n> quote me please.\n\n")));
    QCOMPARE(reply.msg->contentType()->mimeType(), QByteArrayLiteral("multipart/alternative"));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: asking for reply"));
    QCOMPARE_OR_DIFF(reply.msg->contents().at(0)->body(), replyStr.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateForwardMultiEmailsAsync()
{
    KMime::Message::Ptr msg = createPlainTestMessageWithMultiEmails();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    QSignalSpy spy(&factory, &MessageFactoryNG::createForwardDone);
    factory.createForwardAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto fw = spy.at(0).at(0).value<KMime::Message::Ptr>();
    QDateTime date = msg->date()->dateTime();
    QString datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1StringView(", ") + QLocale::system().toString(date.time(), QLocale::LongFormat);

    QString fwdMsg = QString::fromLatin1(
        "From: another <another@another.com>\n"
        "Date: %2\n"
        "X-KMail-Transport: 0\n"
        "MIME-Version: 1.0\n"
        "Subject: Fwd: Test Email Subject\n"
        "Content-Type: text/plain; charset=\"UTF-8\"\n"
        "Content-Transfer-Encoding: 8Bit\n"
        "X-KMail-Link-Message: 0\n"
        "X-KMail-Link-Type: forward\n"
        "\n"
        "----------  Forwarded Message  ----------\n"
        "\n"
        "Subject: Test Email Subject\n"
        "Date: %1\n"
        "From: me@me.me\n"
        "To: you@you.you, you2@you.you\n"
        "CC: cc@cc.cc, cc2@cc.cc\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.\n"
        "-----------------------------------------");
    fwdMsg = fwdMsg.arg(datetime).arg(fw->date()->asUnicodeString());

    QCOMPARE(fw->subject()->asUnicodeString(), QStringLiteral("Fwd: Test Email Subject"));
    QCOMPARE_OR_DIFF(fw->encodedContent(), fwdMsg.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateForwardAsync()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);
    QSignalSpy spy(&factory, &MessageFactoryNG::createForwardDone);
    factory.createForwardAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    auto fw = spy.at(0).at(0).value<KMime::Message::Ptr>();

    QDateTime date = msg->date()->dateTime();
    QString datetime = QLocale::system().toString(date.date(), QLocale::LongFormat);
    datetime += QLatin1StringView(", ") + QLocale::system().toString(date.time(), QLocale::LongFormat);

    QString fwdMsg = QString::fromLatin1(
        "From: another <another@another.com>\n"
        "Date: %2\n"
        "X-KMail-Transport: 0\n"
        "MIME-Version: 1.0\n"
        "Subject: Fwd: Test Email Subject\n"
        "Content-Type: text/plain; charset=\"UTF-8\"\n"
        "Content-Transfer-Encoding: 8Bit\n"
        "X-KMail-Link-Message: 0\n"
        "X-KMail-Link-Type: forward\n"
        "\n"
        "----------  Forwarded Message  ----------\n"
        "\n"
        "Subject: Test Email Subject\n"
        "Date: %1\n"
        "From: me@me.me\n"
        "To: you@you.you\n"
        "CC: cc@cc.cc\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.\n"
        "-----------------------------------------");
    fwdMsg = fwdMsg.arg(datetime).arg(fw->date()->asUnicodeString());

    QCOMPARE(fw->subject()->asUnicodeString(), QStringLiteral("Fwd: Test Email Subject"));
    QCOMPARE_OR_DIFF(fw->encodedContent(), fwdMsg.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateRedirectToAndCCAndBCC()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);

    QString redirectTo = QStringLiteral("redir@redir.com");
    QString redirectCc = QStringLiteral("redircc@redircc.com, redircc2@redircc.com");
    QString redirectBcc = QStringLiteral("redirbcc@redirbcc.com, redirbcc2@redirbcc.com");
    KMime::Message::Ptr rdir = factory.createRedirect(redirectTo, redirectCc, redirectBcc);

    QString datetime = rdir->date()->asUnicodeString();

    const QRegularExpression rx(QStringLiteral("Resent-Message-ID: ([^\n]*)"));
    const QRegularExpressionMatch rxMatch = rx.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxMatch.hasMatch());

    const QRegularExpression rxmessageid(QStringLiteral("Message-ID: ([^\n]+)"));
    const QRegularExpressionMatch rxmessageidMatch = rxmessageid.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxmessageidMatch.hasMatch());

    QString baseline = QString::fromLatin1(
        "From: me@me.me\n"
        "Cc: cc@cc.cc\n"
        "Bcc: bcc@bcc.bcc\n"
        "Subject: Test Email Subject\n"
        "Date: %1\n"
        "X-KMail-Transport: 0\n"
        "Message-ID: %2\n"
        "Disposition-Notification-To: me@me.me\n"
        "MIME-Version: 1.0\n"
        "Content-Transfer-Encoding: 7Bit\n"
        "Content-Type: text/plain; charset=\"utf-8\"\n"
        "Resent-Message-ID: %3\n"
        "Resent-Date: %4\n"
        "Resent-From: %5\n"
        "To: you@you.you\n"
        "Resent-To: redir@redir.com\n"
        "Resent-Cc: redircc@redircc.com, redircc2@redircc.com\n"
        "Resent-Bcc: redirbcc@redirbcc.com, redirbcc2@redirbcc.com\n"
        "X-KMail-Redirect-From: me@me.me (by way of another <another@another.com>)\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.");
    baseline =
        baseline.arg(datetime).arg(rxmessageidMatch.captured(1)).arg(rxMatch.captured(1)).arg(datetime).arg(QStringLiteral("another <another@another.com>"));

    QCOMPARE(rdir->subject()->asUnicodeString(), QStringLiteral("Test Email Subject"));
    QCOMPARE_OR_DIFF(rdir->encodedContent(), baseline.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateRedirectToAndCC()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);

    QString redirectTo = QStringLiteral("redir@redir.com");
    QString redirectCc = QStringLiteral("redircc@redircc.com, redircc2@redircc.com");
    KMime::Message::Ptr rdir = factory.createRedirect(redirectTo, redirectCc);

    QString datetime = rdir->date()->asUnicodeString();

    const QRegularExpression rx(QStringLiteral("Resent-Message-ID: ([^\n]*)"));
    const QRegularExpressionMatch rxMatch = rx.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxMatch.hasMatch());

    const QRegularExpression rxmessageid(QStringLiteral("Message-ID: ([^\n]+)"));
    const QRegularExpressionMatch rxmessageidMatch = rxmessageid.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxmessageidMatch.hasMatch());

    // qWarning() << "messageid:" << rxmessageid.cap(1) << "(" << rdir->head() << ")";
    QString baseline = QString::fromLatin1(
        "From: me@me.me\n"
        "Cc: cc@cc.cc\n"
        "Bcc: bcc@bcc.bcc\n"
        "Subject: Test Email Subject\n"
        "Date: %1\n"
        "X-KMail-Transport: 0\n"
        "Message-ID: %2\n"
        "Disposition-Notification-To: me@me.me\n"
        "MIME-Version: 1.0\n"
        "Content-Transfer-Encoding: 7Bit\n"
        "Content-Type: text/plain; charset=\"utf-8\"\n"
        "Resent-Message-ID: %3\n"
        "Resent-Date: %4\n"
        "Resent-From: %5\n"
        "To: you@you.you\n"
        "Resent-To: redir@redir.com\n"
        "Resent-Cc: redircc@redircc.com, redircc2@redircc.com\n"
        "X-KMail-Redirect-From: me@me.me (by way of another <another@another.com>)\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.");
    baseline =
        baseline.arg(datetime).arg(rxmessageidMatch.captured(1)).arg(rxMatch.captured(1)).arg(datetime).arg(QStringLiteral("another <another@another.com>"));

    QCOMPARE(rdir->subject()->asUnicodeString(), QStringLiteral("Test Email Subject"));
    QCOMPARE_OR_DIFF(rdir->encodedContent(), baseline.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateRedirect()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);

    QString redirectTo = QStringLiteral("redir@redir.com");
    KMime::Message::Ptr rdir = factory.createRedirect(redirectTo);

    QString datetime = rdir->date()->asUnicodeString();

    const QRegularExpression rx(QStringLiteral("Resent-Message-ID: ([^\n]*)"));
    const QRegularExpressionMatch rxMatch = rx.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxMatch.hasMatch());

    const QRegularExpression rxmessageid(QStringLiteral("Message-ID: ([^\n]+)"));
    const QRegularExpressionMatch rxmessageidMatch = rxmessageid.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxmessageidMatch.hasMatch());

    QString baseline = QString::fromLatin1(
        "From: me@me.me\n"
        "Cc: cc@cc.cc\n"
        "Bcc: bcc@bcc.bcc\n"
        "Subject: Test Email Subject\n"
        "Date: %1\n"
        "X-KMail-Transport: 0\n"
        "Message-ID: %2\n"
        "Disposition-Notification-To: me@me.me\n"
        "MIME-Version: 1.0\n"
        "Content-Transfer-Encoding: 7Bit\n"
        "Content-Type: text/plain; charset=\"utf-8\"\n"
        "Resent-Message-ID: %3\n"
        "Resent-Date: %4\n"
        "Resent-From: %5\n"
        "To: you@you.you\n"
        "Resent-To: redir@redir.com\n"
        "X-KMail-Redirect-From: me@me.me (by way of another <another@another.com>)\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.");
    baseline =
        baseline.arg(datetime).arg(rxmessageidMatch.captured(1)).arg(rxMatch.captured(1)).arg(datetime).arg(QStringLiteral("another <another@another.com>"));

    QCOMPARE(rdir->subject()->asUnicodeString(), QStringLiteral("Test Email Subject"));
    QCOMPARE_OR_DIFF(rdir->encodedContent(), baseline.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateResend()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);
    factory.setIdentityManager(mIdentMan);

    KMime::Message::Ptr rdir = factory.createResend();

    QString datetime = rdir->date()->asUnicodeString();

    const QRegularExpression rx(QStringLiteral("Resent-Message-ID: ([^\n]*)"));
    const QRegularExpressionMatch rxMatch = rx.match(QString::fromLatin1(rdir->head()));
    QVERIFY(!rxMatch.hasMatch());

    const QRegularExpression rxmessageid(QStringLiteral("Message-ID: ([^\n]+)"));
    const QRegularExpressionMatch rxmessageidMatch = rxmessageid.match(QString::fromLatin1(rdir->head()));
    QVERIFY(rxmessageidMatch.hasMatch());

    QString baseline = QString::fromLatin1(
        "From: me@me.me\n"
        "To: %1\n"
        "Cc: cc@cc.cc\n"
        "Bcc: bcc@bcc.bcc\n"
        "Subject: Test Email Subject\n"
        "Date: %2\n"
        "X-KMail-Transport: 0\n"
        "Message-ID: %3\n"
        "Disposition-Notification-To: me@me.me\n"
        "MIME-Version: 1.0\n"
        "Content-Transfer-Encoding: 7Bit\n"
        "Content-Type: text/plain; charset=\"utf-8\"\n"
        "\n"
        "All happy families are alike; each unhappy family is unhappy in its own way.");
    baseline = baseline.arg(msg->to()->asUnicodeString()).arg(datetime).arg(rxmessageidMatch.captured(1));

    QCOMPARE(rdir->subject()->asUnicodeString(), QStringLiteral("Test Email Subject"));
    QCOMPARE_OR_DIFF(rdir->encodedContent(), baseline.toUtf8());
    msg.clear();
}

void MessageFactoryTest::testCreateMDN()
{
    KMime::Message::Ptr msg = createPlainTestMessage();

    MessageFactoryNG factory(msg, 0);

    factory.setIdentityManager(mIdentMan);

    KMime::Message::Ptr mdn = factory.createMDN(KMime::MDN::AutomaticAction, KMime::MDN::Displayed, KMime::MDN::SentAutomatically);

    QVERIFY(mdn.data());

    QString mdnContent = QString::fromLatin1(
        "The message sent on %1 to %2 with subject \"%3\" has been displayed. "
        "This is no guarantee that the message has been read or understood.");
    mdnContent = mdnContent.arg(MessageCore::DateFormatter::formatDate(MessageCore::DateFormatter::Localized, msg->date()->dateTime()))
                     .arg(msg->to()->asUnicodeString(), msg->subject()->asUnicodeString());

    QCOMPARE_OR_DIFF(Util::findTypeInMessage(mdn.data(), "multipart", "report")->contents().at(0)->body(), mdnContent.toUtf8());
    msg.clear();
}

KMime::Message::Ptr MessageFactoryTest::createPlainTestMessage()
{
    auto composer = new Composer;
    composer->infoPart()->setFrom(QStringLiteral("me@me.me"));
    composer->infoPart()->setTo(QStringList(QStringLiteral("you@you.you")));
    composer->infoPart()->setCc(QStringList(QStringLiteral("cc@cc.cc")));
    composer->infoPart()->setBcc(QStringList(QStringLiteral("bcc@bcc.bcc")));
    composer->textPart()->setWrappedPlainText(QStringLiteral("All happy families are alike; each unhappy family is unhappy in its own way."));
    composer->infoPart()->setSubject(QStringLiteral("Test Email Subject"));
    composer->globalPart()->setMDNRequested(true);
    composer->exec();

    KMime::Message::Ptr message = KMime::Message::Ptr(composer->resultMessages().first());
    delete composer;

    return message;
}

KMime::Message::Ptr MessageFactoryTest::createPlainTestMessageWithMultiEmails()
{
    auto composer = new Composer;
    composer->infoPart()->setFrom(QStringLiteral("me@me.me"));
    composer->infoPart()->setTo(QStringList() << QStringLiteral("you@you.you") << QStringLiteral("you2@you.you"));
    composer->infoPart()->setCc(QStringList() << QStringLiteral("cc@cc.cc") << QStringLiteral("cc2@cc.cc"));
    composer->infoPart()->setBcc(QStringList() << QStringLiteral("bcc@bcc.bcc") << QStringLiteral("bcc2@bcc.bcc"));
    composer->textPart()->setWrappedPlainText(QStringLiteral("All happy families are alike; each unhappy family is unhappy in its own way."));
    composer->infoPart()->setSubject(QStringLiteral("Test Email Subject"));
    composer->globalPart()->setMDNRequested(true);
    composer->exec();

    const KMime::Message::Ptr message = KMime::Message::Ptr(composer->resultMessages().constFirst());
    delete composer;

    return message;
}

void MessageFactoryTest::test_multipartAlternative_data()
{
    QTest::addColumn<QString>("mailFileName");
    QTest::addColumn<int>("contentAt");
    QTest::addColumn<QString>("selection");
    QTest::addColumn<QString>("expected");

    QDir dir(QStringLiteral(MAIL_DATA_DIR));
    const QStringList lst = dir.entryList(QStringList(QStringLiteral("plain_message.mbox")), QDir::Files | QDir::Readable | QDir::NoSymLinks);
    for (const QString &file : lst) {
        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << 0 << ""
                                                   << "> This *is* the *message* text *from* Sudhendu Kumar<dontspamme@yoohoo.com>\n"
                                                      "> \n"
                                                      "> --\n"
                                                      "> Thanks & Regards\n"
                                                      "> Sudhendu Kumar";
        QTest::newRow(file.toLatin1().constData())
            << QString(dir.path() + QLatin1Char('/') + file) << 1 << ""
            << "<html><head></head><body>"
               "<blockquote>This <i>is</i> the <b>message</b> text <u>from</u> Sudhendu Kumar&lt;dontspamme@yoohoo.com&gt;<br>"
               "<br>-- <br>Thanks &amp; Regards<br>Sudhendu Kumar<br><br></blockquote><br/></body></html>";

        QTest::newRow(file.toLatin1().constData()) << QString(dir.path() + QLatin1Char('/') + file) << 0 << "This *is* the *message* text *from*"
                                                   << "> This *is* the *message* text *from*";

        QTest::newRow(file.toLatin1().constData())
            << QString(dir.path() + QLatin1Char('/') + file) << 1 << "This *is* the *message* text *from*"
            << "<html><head></head><body><blockquote>This *is* the *message* text *from*</blockquote><br/></body></html>";
    }
}

void MessageFactoryTest::test_multipartAlternative()
{
    QFETCH(QString, mailFileName);
    QFETCH(int, contentAt);
    QFETCH(QString, selection);
    QFETCH(QString, expected);

    KMime::Message::Ptr origMsg = Test::loadMessage(mailFileName);

    MessageFactoryNG factory(origMsg, 0);
    factory.setIdentityManager(mIdentMan);
    factory.setSelection(selection);
    factory.setQuote(true);
    factory.setReplyStrategy(ReplyAll);
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QStringLiteral("%QUOTE"));
    TemplateParser::TemplateParserSettings::self()->setReplyUsingVisualFormat(true);
    factory.setReplyAsHtml(true);

    QString str;
    str = TemplateParser::TemplateParserSettings::self()->templateReplyAll();
    factory.setTemplate(str);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);

    factory.createReplyAsync();
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    auto reply = spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>();
    reply.replyAll = true;
    QCOMPARE(reply.msg->contentType()->mimeType(), QByteArrayLiteral("multipart/alternative"));
    QCOMPARE(reply.msg->subject()->asUnicodeString(), QLatin1StringView("Re: Plain Message Test"));
    QCOMPARE(reply.msg->contents().at(contentAt)->encodedBody().data(), expected.toUtf8().data());
    origMsg.clear();
}

KMime::Message::Ptr MessageFactoryTest::createReplyAllForMessage(KMime::Message::Ptr origMsg)
{
    MessageFactoryNG factory(origMsg, 0);
    factory.setIdentityManager(mIdentMan);
    factory.setQuote(true);
    factory.setReplyStrategy(ReplyAll);

    QSignalSpy spy(&factory, &MessageFactoryNG::createReplyDone);
    factory.createReplyAsync();
    spy.wait();
    return spy.at(0).at(0).value<MessageComposer::MessageFactoryNG::MessageReply>().msg;
}

void MessageFactoryTest::testCreateReplyWithForcedCharset()
{
    KMime::Message::Ptr origMsg(new KMime::Message);
    QByteArray origMail =
        "From: from@example.com\n"
        "To: to@example.com\n"
        "Sender: from@example.com\n"
        "Subject: Test\n"
        "Content-Type: text/plain; charset=iso-8859-1\n"
        "\n"
        "Test \xC4\n";
    origMsg->setContent(origMail);
    origMsg->parse();

    QCOMPARE(origMsg->contentType()->charset(), QByteArray("iso-8859-1"));
    QCOMPARE(origMsg->body(), QByteArray("Test \xC4\n"));

    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QString::fromUtf8("%QUOTE"));

    // The Euro symbol can't be encoded in ISO-8859-1, so it has to choose UTF-8 instead
    TemplateParser::TemplateParserSettings::self()->setTemplateReplyAll(QString::fromUtf8("\xE2\x82\xAC%QUOTE"));

    {
        // Use the preferred charset, UTF-8
        auto msg = createReplyAllForMessage(origMsg);
        QCOMPARE(msg->contentType()->charset(), QByteArray("UTF-8"));
        QCOMPARE(msg->body(), QByteArray("\xE2\x82\xAC> Test \xC3\x84"));
    }
}

#include "moc_messagefactoryngtest.cpp"
