/*
  Copyright (C) 2019 Glen Ditchfield <GJDitchfield@acm.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "replystrategytest.h"

#include <QSignalSpy>
#include <QStandardPaths>
#include <QStringLiteral>
#include <QTest>

#include <KIdentityManagement/KIdentityManagement/Identity>
#include <KIdentityManagement/KIdentityManagement/IdentityManager>

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/TextPart>

const auto defaultAddress {QStringLiteral("default@example.org")};
const auto nondefaultAddress {QStringLiteral("nondefault@example.com")};
const auto friend1Address {QStringLiteral("friend1@example.net")};
const auto friend2Address {QStringLiteral("friend2@example.net")};
const auto replyAddress {QStringLiteral("reply@example.com")};
const auto followupAddress {QStringLiteral("followup@example.org")};
const auto listAddress {QStringLiteral("list@example.com")};
const auto mailReplyAddress {QStringLiteral("mailreply@example.com")};
const QStringList nobody {};

static inline const QStringList only(const QString &address)
{
    return QStringList {address};
}

static inline const QStringList both(const QString &address1, const QString &address2)
{
    return QStringList {address1, address2};
}

using namespace MessageComposer;

static KMime::Message::Ptr basicMessage(const QString &fromAddress, const QStringList &toAddresses)
{
    Composer composer;
    composer.infoPart()->setFrom(fromAddress);
    composer.infoPart()->setTo(toAddresses);
    composer.infoPart()->setSubject(QStringLiteral("Test Email Subject"));
    composer.textPart()->setWrappedPlainText(QStringLiteral("Test email body."));
    composer.exec();

    return composer.resultMessages().first();
}

#define COMPARE_ADDRESSES(actual, expected) \
    if (!compareAddresses(actual, expected)) { \
        QFAIL(qPrintable(QStringLiteral("%1 is \"%2\"") \
                         .arg(QString::fromLatin1(#actual), actual->displayString()))); \
        return; \
    }

template<class T>
bool compareAddresses(const T *actual, const QStringList &expected)
{
    auto addresses {actual->addresses()};
    if (addresses.length() != expected.length()) {
        return false;
    }
    for (const auto &e : expected) {
        if (!addresses.contains(e.toLatin1())) {
            return false;
        }
    }
    return true;
}

ReplyStrategyTest::ReplyStrategyTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

ReplyStrategyTest::~ReplyStrategyTest()
{
    // Workaround QTestLib not flushing deleteLater()s on exit, which
    // leads to WebEngine asserts (view not deleted)
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

void ReplyStrategyTest::initTestCase()
{
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
                  + QDir::separator() + QStringLiteral("emailidentities"));
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)
                  + QDir::separator() + QStringLiteral("emaildefaults"));

    mIdentityManager = new KIdentityManagement::IdentityManager;

    auto homeIdentity = mIdentityManager->newFromExisting(KIdentityManagement::Identity {
        QStringLiteral("Home Identity"),
        QStringLiteral("Full Home Name"),
        defaultAddress});
    QVERIFY(mIdentityManager->setAsDefault(homeIdentity.uoid()));

    auto workIdentity = mIdentityManager->newFromExisting(KIdentityManagement::Identity {
        QStringLiteral("Work Identity"),
        QStringLiteral("Full Work Name"),
        nondefaultAddress});

    mIdentityManager->commit();
}

void ReplyStrategyTest::cleanupTestCase()
{
    delete mIdentityManager;
}

KMime::Message::Ptr ReplyStrategyTest::makeReply(const KMime::Message::Ptr &original, const ReplyStrategy strategy)
{
    MessageFactoryNG factory {original, 0};
    factory.setReplyStrategy(strategy);
    factory.setIdentityManager(mIdentityManager);
    QSignalSpy spy {&factory, &MessageFactoryNG::createReplyDone};
    factory.createReplyAsync();
    KMime::Message::Ptr result {nullptr};
    [&]{
        QVERIFY(spy.wait());
        QCOMPARE(spy.count(), 1);
        result = spy.at(0).at(0).value<MessageFactoryNG::MessageReply>().msg;
    } ();
    return result;
}

void ReplyStrategyTest::testReply_data()
{
    QTest::addColumn<QString>("oFrom");     // Original message's From address.
    QTest::addColumn<QStringList>("oTo");   // Original message's To addresses.
    QTest::addColumn<QStringList>("oCc");   // Original message's CC addresses.
    QTest::addColumn<QStringList>("oRT");   // Original message's Reply-To addresses.
    QTest::addColumn<QStringList>("oMFT");  // Original message's Mail-Followup-To addresses.
    QTest::addColumn<QString>("oLP");       // Original message's List-Post address.
    QTest::addColumn<QStringList>("oMRT");  // Original message's Mail-Reply-To addresses.
    QTest::addColumn<int>("strategy");      // ReplyStrategy (passed as an int).
    QTest::addColumn<QString>("rFrom");     // Reply's expected From address.
    QTest::addColumn<QStringList>("rTo");   // Reply's expected To addresses.
    QTest::addColumn<QStringList>("rCc");   // Reply's expected CC addresses.

    // Smart Replies
    // -------------
    // Smart Reply does not set CC headers.  (Compare ReplyAll.)
    // ReplySmart uses Mail-Reply-To, Reply-To, or From (in that order)
    // for the original's author's address, if List-Post is absent.
    QTest::newRow("ReplySmart, from someone to default identity")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << nobody << nobody << QString() << nobody
        << (int)ReplySmart << defaultAddress << only(friend1Address) << nobody;
    QTest::newRow("ReplySmart, from someone to non-default identity")
        << friend1Address << both(friend2Address, nondefaultAddress) << only(defaultAddress)
        << nobody << nobody << QString() << nobody
        << (int)ReplySmart << nondefaultAddress << only(friend1Address) << nobody;
    QTest::newRow("ReplySmart, from someone with Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << both(replyAddress, friend2Address) << nobody << QString() << nobody
        << (int)ReplySmart << defaultAddress << both(friend2Address, replyAddress) << nobody;
    QTest::newRow("ReplySmart, from someone with Mail-Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << nobody << QString() << only(mailReplyAddress)
        << (int)ReplySmart << defaultAddress << only(mailReplyAddress) << nobody;

    // If the original message was _from_ the user _to_ another person (the
    // reverse of the usual direction), a smart reply goes to the other person.
    // Therefore Mail-Reply-To and Reply-To are ignored.
    // The reply is assumed to add to the original message.
    QTest::newRow("ReplySmart, from default identity to someone")
        << defaultAddress << only(friend1Address) << only(friend2Address)
        << nobody << nobody << QString() << nobody
        << (int)ReplySmart << defaultAddress << only(friend1Address) << nobody;
    QTest::newRow("ReplySmart, from default identity with Reply-To to someone")
        << defaultAddress << only(friend1Address) << only(friend2Address)
        << only(replyAddress) << nobody << QString() << only(mailReplyAddress)
        << (int)ReplySmart << defaultAddress << only(friend1Address) << nobody;

    // If the original message was from one of the user's identities to another
    // identity (i.e., between two of the user's mail accounts), a smart reply
    // goes back to the sending identity.
    QTest::newRow("ReplySmart, between identities")
        << defaultAddress << only(nondefaultAddress) << only(friend2Address)
        << nobody << nobody << QString() << nobody
        << (int)ReplySmart << nondefaultAddress << only(defaultAddress) << nobody;

    // If the original message appears to be from a mailing list, smart replies
    // go to the Mail-Followup-To, Reply-To, or List-Post addresses, in that
    // order of preference.
    QTest::newRow("ReplySmart, from list with Mail-Followup-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << only(followupAddress) << listAddress << nobody
        << (int)ReplySmart << defaultAddress << only(followupAddress) << nobody;
    QTest::newRow("ReplySmart, from list with Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << nobody << listAddress << nobody
        << (int)ReplySmart << defaultAddress << only(replyAddress) << nobody;
    QTest::newRow("ReplySmart, from list with List-Post")
        << friend1Address << only(nondefaultAddress) << only(friend2Address)
        << nobody << nobody << listAddress << only(mailReplyAddress)
        << (int)ReplySmart << nondefaultAddress << only(listAddress) << nobody;

    // Replies to Mailing Lists
    // ------------------------
    // If the original message has a Mail-Followup-To header, replies to the list
    // go to the followup address, in preference to List-Post and Reply-To.
    // Cc and Mail-Reply-To are ignored.
    QTest::newRow("ReplyList, from list with Mail-Followup-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << only(followupAddress) << listAddress << only(mailReplyAddress)
        << (int)ReplyList << defaultAddress << only(followupAddress) << nobody;

    // If the original message has a List-Post header, replies to the list
    // go to that address, in preference to Reply-To.
    QTest::newRow("ReplyList, from list with List-Post")
        << friend1Address << only(defaultAddress) << nobody
        << only(replyAddress) << nobody << listAddress << nobody
        << (int)ReplyList << defaultAddress << only(listAddress) << nobody;

    // If the original message has just a Reply-To header, assume the list
    // [munges Reply-To](https://www.gnu.org/software/mailman/mailman-admin/node11.html)
    /// and send the reply to that address.
    QTest::newRow("ReplyList, from list with Reply-To")
        << friend1Address << only(defaultAddress) << nobody
        << only(replyAddress) << nobody << QString() << nobody
        << (int)ReplyList << defaultAddress << only(replyAddress) << nobody;

    // If the original message has neither Mail-Followup-To, List-Post, nor
    // Reply-To headers, replies to the list do not choose a To address.
    QTest::newRow("ReplyList, from list with no headers")
        << friend1Address << only(defaultAddress) << nobody
        << nobody << nobody << QString() << nobody
        << (int)ReplyList << defaultAddress << nobody << nobody;

    // Replies to All
    // --------------
    // ReplyAll adds CC addresses to the reply for the original's recipients,
    // except for the user's identities.
    QTest::newRow("ReplyAll, with Cc in original")
        << friend1Address << only(defaultAddress) << both(friend2Address, nondefaultAddress)
        << nobody << nobody << QString() << nobody
        << (int)ReplyAll << defaultAddress << only(friend1Address) << only(friend2Address);
    QTest::newRow("ReplyAll, with multiple To addresses in original")
        << friend1Address << both(friend2Address, nondefaultAddress) << only(defaultAddress)
        << nobody << nobody << QString() << nobody
        << (int)ReplyAll << nondefaultAddress << only(friend1Address) << only(friend2Address);
    QTest::newRow("ReplyAll, with Reply-To in original")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << nobody << QString() << nobody
        << (int)ReplyAll << defaultAddress << only(replyAddress) << only(friend2Address);

    // If the original passed through a mailing list, ReplyAll replies to the
    // list, preferring Mail-Followup-To over List-Post as the list address.
    // It CCs the author, using Mail-Reply-To, Reply-To, or From (in that order).
    QTest::newRow("ReplyAll, from list with List-Post")
        << friend1Address << only(nondefaultAddress) << only(friend2Address)
        << nobody << nobody << listAddress << nobody
        << (int)ReplyAll << nondefaultAddress << only(listAddress) << both(friend1Address, friend2Address);

    // If Reply-To is the same as List-Post, ReplyAll ignores it and uses
    // From, because the mailing list munged Reply-To.
    QTest::newRow("ReplyAll, from list that munges Reply-To")
        << friend1Address << only(defaultAddress) << nobody
        << only(listAddress) << nobody << listAddress << nobody
        << (int)ReplyAll << defaultAddress << only(listAddress) << only(friend1Address);

    // Reply to Author
    // ---------------
    // ReplyAuthor ignores Cc, and replies to the Mail-Reply-To, Reply-To, or
    // From addresses, in that order of preference, if List-Post is absent.
    QTest::newRow("ReplyAuthor, no special headers")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << nobody << nobody << QString() << nobody
        << (int)ReplyAuthor << defaultAddress << only(friend1Address) << nobody;
    QTest::newRow("ReplyAuthor, from someone with Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << nobody << QString() << nobody
        << (int)ReplyAuthor << defaultAddress << only(replyAddress) << nobody;
    QTest::newRow("ReplyAuthor, from someone with Mail-Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << nobody << QString() << only(mailReplyAddress)
        << (int)ReplyAuthor << defaultAddress << only(mailReplyAddress) << nobody;

    // If Reply-To is the same as List-Post, ReplyAuthor ignores it and uses
    // From, because the mailing list munged Reply-To.
    QTest::newRow("ReplyAuthor, from list that munges Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(listAddress) << nobody << listAddress << nobody
        << (int)ReplyAuthor << defaultAddress << only(friend1Address) << nobody;

    // If Reply-To contains List-Post, ReplyAuthor uses the other reply
    // addresses, because the mailing list didn't completely munge Reply-To.
    QTest::newRow("ReplyAuthor, from list that lightly munges Reply-To")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << both(listAddress, replyAddress) << nobody << listAddress << nobody
        << (int)ReplyAuthor << defaultAddress << only(replyAddress) << nobody;

    // Reply to None
    // -------------
    // ReplyNone ignores all possible headers and does not choose a To address.
    QTest::newRow("ReplyNone")
        << friend1Address << only(defaultAddress) << only(friend2Address)
        << only(replyAddress) << only(followupAddress) << listAddress << only(mailReplyAddress)
        << (int)ReplyNone << defaultAddress << nobody << nobody;
}

void ReplyStrategyTest::testReply()
{
    QFETCH(const QString, oFrom);
    QFETCH(const QStringList, oTo);
    QFETCH(const QStringList, oCc);
    QFETCH(const QStringList, oRT);
    QFETCH(const QStringList, oMFT);
    QFETCH(const QString, oLP);
    QFETCH(const QStringList, oMRT);
    QFETCH(const int, strategy);
    QFETCH(const QString, rFrom);
    QFETCH(const QStringList, rTo);
    QFETCH(const QStringList, rCc);

    auto original {basicMessage(oFrom, oTo)};
    if (!oCc.isEmpty()) {
        auto cc {new KMime::Headers::Cc};
        for (const auto &a : oCc) {
            cc->addAddress(a.toLatin1());
        }
        original->setHeader(cc);
    }
    if (!oRT.isEmpty()) {
        auto replyTo {new KMime::Headers::ReplyTo};
        for (const auto &a : oRT) {
            replyTo->addAddress(a.toLatin1());
        }
        original->setHeader(replyTo);
    }
    if (!oMFT.isEmpty()) {
        auto mailFollowupTo = new KMime::Headers::Generic("Mail-Followup-To");
        mailFollowupTo->from7BitString(oMFT.join(QLatin1Char(',')).toLatin1());
        original->setHeader(mailFollowupTo);
    }
    if (!oLP.isEmpty()) {
        auto listPost = new KMime::Headers::Generic("List-Post");
        listPost->from7BitString("<mailto:" + oLP.toLatin1() + ">");
        original->setHeader(listPost);
    }
    if (!oMRT.isEmpty()) {
        auto mailReplyTo = new KMime::Headers::Generic("Mail-Reply-To");
        mailReplyTo->from7BitString(oMRT.join(QLatin1Char(',')).toLatin1());
        original->setHeader(mailReplyTo);
    }

    if (auto reply = makeReply(original, (ReplyStrategy)strategy)) {
        COMPARE_ADDRESSES(reply->from(), only(rFrom));
        COMPARE_ADDRESSES(reply->to(), rTo);
        COMPARE_ADDRESSES(reply->cc(), rCc);
    }
}

QTEST_MAIN(ReplyStrategyTest)
