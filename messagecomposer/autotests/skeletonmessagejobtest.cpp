/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "skeletonmessagejobtest.h"

#include <QDebug>
#include <QTest>

#include <KMime/Message>

#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/SkeletonMessageJob>

using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

QTEST_MAIN(SkeletonMessageJobTest)

void SkeletonMessageJobTest::testSubject_data()
{
    QTest::addColumn<QString>("subject");

    QTest::newRow("simple subject") << u"Antaa virrata sateen..."_s;
    QTest::newRow("non-ascii subject") << u"Muzicologă în bej, vând whisky și tequila, preț fix."_s;
    // NOTE: This works fine, but shows ??s in the debug output.  Why?
}

void SkeletonMessageJobTest::testSubject()
{
    // An InfoPart should belong to a Composer, even if we don't use the composer itself.
    ComposerJob composerJob;
    InfoPart *infoPart = composerJob.infoPart();
    GlobalPart *globalPart = composerJob.globalPart();
    Q_ASSERT(infoPart);

    QFETCH(QString, subject);
    // qDebug() << subject;
    infoPart->setSubject(subject);
    auto sjob = new SkeletonMessageJob(infoPart, globalPart, &composerJob);
    QVERIFY(sjob->exec());
    KMime::Message *message = sjob->message();
    QVERIFY(message->subject(false));
    qDebug() << message->subject(false)->asUnicodeString();
    QCOMPARE(subject, message->subject(false)->asUnicodeString());
    delete message;
}

void SkeletonMessageJobTest::testAddresses_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QStringList>("replyto");
    QTest::addColumn<QStringList>("to");
    QTest::addColumn<QStringList>("cc");
    QTest::addColumn<QStringList>("bcc");

    {
        QString from = u"one@example.com"_s;
        QStringList to;
        to << u"two@example.com"_s;
        QStringList cc;
        cc << u"three@example.com"_s;
        QStringList bcc;
        bcc << u"four@example.com"_s;
        QString replyto = u"five@example.com"_s;

        QTest::newRow("simple single address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = u"one@example.com"_s;
        QStringList to;
        to << u"two@example.com"_s;
        to << u"two.two@example.com"_s;
        QStringList cc;
        cc << u"three@example.com"_s;
        cc << u"three.three@example.com"_s;
        QStringList bcc;
        bcc << u"four@example.com"_s;
        bcc << u"four.four@example.com"_s;
        QString replyto = u"five@example.com"_s;

        QTest::newRow("simple multi address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = u"Me <one@example.com>"_s;
        QStringList to;
        to << u"You <two@example.com>"_s;
        to << u"two.two@example.com"_s;
        QStringList cc;
        cc << u"And you <three@example.com>"_s;
        cc << u"three.three@example.com"_s;
        QStringList bcc;
        bcc << u"And you too <four@example.com>"_s;
        bcc << u"four.four@example.com"_s;
        QString replyto = u"You over there <five@example.com>"_s;

        QTest::newRow("named multi address") << from << QStringList{replyto} << to << cc << bcc;
    }

    {
        QString from = u"Şîşkin <one@example.com>"_s;
        QStringList to;
        to << u"Ivan Turbincă <two@example.com>"_s;
        to << u"two.two@example.com"_s;
        QStringList cc;
        cc << u"Luceafărul <three@example.com>"_s;
        cc << u"three.three@example.com"_s;
        QStringList bcc;
        bcc << u"Zburătorul <four@example.com>"_s;
        bcc << u"four.four@example.com"_s;
        QString replyto = u"Şîşzbură <five@example.com>"_s;

        QTest::newRow("non-ascii named multi address") << from << QStringList{replyto} << to << cc << bcc;
    }
}

void SkeletonMessageJobTest::testAddresses()
{
    // An InfoPart should belong to a Composer, even if we don't use the composer itself.
    ComposerJob composerJob;
    InfoPart *infoPart = composerJob.infoPart();
    GlobalPart *globalPart = composerJob.globalPart();
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
    auto sjob = new SkeletonMessageJob(infoPart, globalPart, &composerJob);
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
        QCOMPARE(replyto.join(u','), message->replyTo()->asUnicodeString());
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
}

void SkeletonMessageJobTest::testMessageID()
{
    ComposerJob composerJob;
    InfoPart *infoPart = composerJob.infoPart();
    infoPart->setFrom(u"test@example.org"_s);
    GlobalPart *globalPart = composerJob.globalPart();
    Q_ASSERT(infoPart);

    auto sjob = new SkeletonMessageJob(infoPart, globalPart, &composerJob);
    QVERIFY(sjob->exec());
    KMime::Message *message = sjob->message();
    QVERIFY(message->messageID(false));
    QVERIFY(!message->messageID(false)->isEmpty());
    delete message;
    delete sjob;
}

#include "moc_skeletonmessagejobtest.cpp"
