/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "skeletonmessagejobtest.h"

#include <QDebug>
#include <QTest>

#include <KMime/Message>

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/SkeletonMessageJob>
using namespace MessageComposer;

QTEST_MAIN(SkeletonMessageJobTest)

void SkeletonMessageJobTest::testSubject_data()
{
    QTest::addColumn<QString>("subject");

    QTest::newRow("simple subject") << QStringLiteral("Antaa virrata sateen...");
    QTest::newRow("non-ascii subject") << QStringLiteral("Muzicologă în bej, vând whisky și tequila, preț fix.");
    // NOTE: This works fine, but shows ??s in the debug output.  Why?
}

void SkeletonMessageJobTest::testSubject()
{
    // An InfoPart should belong to a Composer, even if we don't use the composer itself.
    auto composer = new Composer;
    InfoPart *infoPart = composer->infoPart();
    GlobalPart *globalPart = composer->globalPart();
    Q_ASSERT(infoPart);

    QFETCH(QString, subject);
    // qDebug() << subject;
    infoPart->setSubject(subject);
    auto sjob = new SkeletonMessageJob(infoPart, globalPart, composer);
    QVERIFY(sjob->exec());
    KMime::Message *message = sjob->message();
    QVERIFY(message->subject(false));
    qDebug() << message->subject(false)->asUnicodeString();
    QCOMPARE(subject, message->subject(false)->asUnicodeString());
    delete message;
    delete composer;
}

void SkeletonMessageJobTest::testAddresses_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QStringList>("replyto");
    QTest::addColumn<QStringList>("to");
    QTest::addColumn<QStringList>("cc");
    QTest::addColumn<QStringList>("bcc");

    {
        QString from = QStringLiteral("one@example.com");
        QStringList to;
        to << QStringLiteral("two@example.com");
        QStringList cc;
        cc << QStringLiteral("three@example.com");
        QStringList bcc;
        bcc << QStringLiteral("four@example.com");
        QString replyto = QStringLiteral("five@example.com");

        QTest::newRow("simple single address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = QStringLiteral("one@example.com");
        QStringList to;
        to << QStringLiteral("two@example.com");
        to << QStringLiteral("two.two@example.com");
        QStringList cc;
        cc << QStringLiteral("three@example.com");
        cc << QStringLiteral("three.three@example.com");
        QStringList bcc;
        bcc << QStringLiteral("four@example.com");
        bcc << QStringLiteral("four.four@example.com");
        QString replyto = QStringLiteral("five@example.com");

        QTest::newRow("simple multi address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = QStringLiteral("Me <one@example.com>");
        QStringList to;
        to << QStringLiteral("You <two@example.com>");
        to << QStringLiteral("two.two@example.com");
        QStringList cc;
        cc << QStringLiteral("And you <three@example.com>");
        cc << QStringLiteral("three.three@example.com");
        QStringList bcc;
        bcc << QStringLiteral("And you too <four@example.com>");
        bcc << QStringLiteral("four.four@example.com");
        QString replyto = QStringLiteral("You over there <five@example.com>");

        QTest::newRow("named multi address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = QStringLiteral("Şîşkin <one@example.com>");
        QStringList to;
        to << QStringLiteral("Ivan Turbincă <two@example.com>");
        to << QStringLiteral("two.two@example.com");
        QStringList cc;
        cc << QStringLiteral("Luceafărul <three@example.com>");
        cc << QStringLiteral("three.three@example.com");
        QStringList bcc;
        bcc << QStringLiteral("Zburătorul <four@example.com>");
        bcc << QStringLiteral("four.four@example.com");
        QString replyto = QStringLiteral("Şîşzbură <five@example.com>");

        QTest::newRow("non-ascii named multi address") << from << QStringList{replyto} << to << cc << bcc;
    }
}

void SkeletonMessageJobTest::testAddresses()
{
    // An InfoPart should belong to a Composer, even if we don't use the composer itself.
    auto composer = new Composer;
    InfoPart *infoPart = composer->infoPart();
    GlobalPart *globalPart = composer->globalPart();
    Q_ASSERT(infoPart);

    QFETCH(QString, from);
    QFETCH(QStringList, replyto);
    QFETCH(QStringList, to);
    QFETCH(QStringList, cc);
    QFETCH(QStringList, bcc);
    infoPart->setFrom(from);
    infoPart->setReplyTo(replyto);
    infoPart->setTo(to);
    infoPart->setCc(cc);
    infoPart->setBcc(bcc);
    auto sjob = new SkeletonMessageJob(infoPart, globalPart, composer);
    QVERIFY(sjob->exec());
    KMime::Message *message = sjob->message();

    {
        QVERIFY(message->from(false));
        // qDebug() << "From:" << message->from()->asUnicodeString();
        QCOMPARE(from, message->from()->asUnicodeString());
    }

    {
        QVERIFY(message->replyTo(false));
        // qDebug() << "Reply-To:" << message->replyTo()->asUnicodeString();
        QCOMPARE(replyto.join(QLatin1Char(',')), message->replyTo()->asUnicodeString());
    }

    {
        QVERIFY(message->to(false));
        // qDebug() << "To:" << message->to()->asUnicodeString();
        const auto mailboxes{message->to(false)->mailboxes()};
        for (const auto &addr : mailboxes) {
            // qDebug() << addr.prettyAddress();
            QVERIFY(to.contains(addr.prettyAddress()));
            to.removeOne(addr.prettyAddress());
        }
        QVERIFY(to.isEmpty());
    }

    {
        QVERIFY(message->cc(false));
        // qDebug() << "Cc:" << message->cc()->asUnicodeString();
        const auto mailboxes{message->cc(false)->mailboxes()};
        for (const auto &addr : mailboxes) {
            // qDebug() << addr.prettyAddress();
            QVERIFY(cc.contains(addr.prettyAddress()));
            cc.removeOne(addr.prettyAddress());
        }
        QVERIFY(cc.isEmpty());
    }

    {
        QVERIFY(message->bcc(false));
        // qDebug() << "Bcc:" << message->bcc()->asUnicodeString();
        const auto mailboxes{message->bcc(false)->mailboxes()};
        for (const auto &addr : mailboxes) {
            // qDebug() << addr.prettyAddress();
            QVERIFY(bcc.contains(addr.prettyAddress()));
            bcc.removeOne(addr.prettyAddress());
        }
        QVERIFY(bcc.isEmpty());
    }
    delete message;
    delete composer;
}

void SkeletonMessageJobTest::testMessageID()
{
    auto composer = new Composer();
    InfoPart *infoPart = composer->infoPart();
    GlobalPart *globalPart = composer->globalPart();
    Q_ASSERT(infoPart);

    auto sjob = new SkeletonMessageJob(infoPart, globalPart, composer);
    QVERIFY(sjob->exec());
    KMime::Message *message = sjob->message();
    QVERIFY(message->messageID(false));
    QVERIFY(!message->messageID(false)->isEmpty());
    delete message;
    delete sjob;
    delete composer;
}

#include "moc_skeletonmessagejobtest.cpp"
