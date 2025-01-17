/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composertest.h"

#include "qtest_messagecomposer.h"

#include <QTest>

#include <KMime/Headers>
using namespace KMime;

#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/TextPart>
using namespace MessageComposer;

#include <MessageCore/AttachmentPart>
using MessageCore::AttachmentPart;

QTEST_MAIN(ComposerTest)

void ComposerTest::testAttachments()
{
    ComposerJob composerJob;
    fillComposerData(&composerJob);
    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    composerJob.addAttachmentPart(attachment);

    QVERIFY(composerJob.exec());
    QCOMPARE(composerJob.resultMessages().size(), 1);
    KMime::Message::Ptr message = composerJob.resultMessages().constFirst();

    // multipart/mixed
    {
        QVERIFY(message->contentType(false));
        QCOMPARE(message->contentType(false)->mimeType(), QByteArray("multipart/mixed"));
        QCOMPARE(message->contents().count(), 2);
        // text/plain
        {
            Content *plain = message->contents().at(0);
            QVERIFY(plain->contentType(false));
            QCOMPARE(plain->contentType(false)->mimeType(), QByteArray("text/plain"));
        }
        // x-some/x-type (attachment)
        {
            Content *plain = message->contents().at(1);
            QVERIFY(plain->contentType(false));
            QCOMPARE(plain->contentType(false)->mimeType(), QByteArray("x-some/x-type"));
        }
    }
}

void ComposerTest::testAutoSave()
{
    ComposerJob composerJob;
    fillComposerData(&composerJob);
    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    composerJob.addAttachmentPart(attachment);

    // This tests if autosave in crash mode works without invoking an event loop, since using an
    // event loop in the crash handler would be a pretty bad idea
    composerJob.setAutoSave(true);
    composerJob.start();
    QVERIFY(composerJob.finished());
    QCOMPARE(composerJob.resultMessages().size(), 1);
}

void ComposerTest::testNonAsciiHeaders()
{
    ComposerJob composerJob;
    fillComposerData(&composerJob);

    const QString mailbox = QStringLiteral(" <bla@example.com>");
    const QString fromDisplayName = QStringLiteral("Hellö");
    const QString toDisplayName = QStringLiteral("æſłĸð");
    const QString ccDisplayName = QStringLiteral("Вася");
    const QString bccDisplayName = QStringLiteral("ĸłſðđøþĸµ»«„¢þµ¢”«ł„·ĸ”");
    const QString replyToDisplayName = QStringLiteral("æĸſłð˝đВасяðæĸđ");
    const QString from = fromDisplayName + mailbox;
    const QString to = toDisplayName + mailbox;
    const QString cc = ccDisplayName + mailbox;
    const QString bcc = bccDisplayName + mailbox;
    const QStringList replyto = QStringList{replyToDisplayName + mailbox};

    composerJob.infoPart()->setFrom(from);
    composerJob.infoPart()->setTo(QStringList() << to);
    composerJob.infoPart()->setCc(QStringList() << cc);
    composerJob.infoPart()->setBcc(QStringList() << bcc);
    composerJob.infoPart()->setReplyTo(replyto);

    QVERIFY(composerJob.exec());
    QCOMPARE(composerJob.resultMessages().size(), 1);
    const KMime::Message::Ptr message = composerJob.resultMessages().constFirst();
    message->assemble();
    message->parse();
    QCOMPARE(message->bcc(false)->displayNames().size(), 1);
    QCOMPARE(message->to(false)->displayNames().size(), 1);
    QCOMPARE(message->cc(false)->displayNames().size(), 1);
    QCOMPARE(message->from(false)->displayNames().size(), 1);
    QCOMPARE(message->replyTo(false)->displayNames().size(), 1);
    QCOMPARE(message->from(false)->displayNames().constFirst(), fromDisplayName);
    QCOMPARE(message->to(false)->displayNames().constFirst(), toDisplayName);
    QCOMPARE(message->cc(false)->displayNames().constFirst(), ccDisplayName);
    QCOMPARE(message->bcc(false)->displayNames().constFirst(), bccDisplayName);
    QCOMPARE(message->replyTo(false)->displayNames().constFirst(), replyToDisplayName);
    message->clear();
}

void ComposerTest::testBug271192()
{
    const QString displayName = QStringLiteral("Интернет-компания example");
    const QString mailbox = QStringLiteral("example@example.com");
    ComposerJob composerJob;
    fillComposerData(&composerJob);
    composerJob.infoPart()->setTo(QStringList() << (displayName + QLatin1StringView(" <") + mailbox + QLatin1StringView(">")));
    QVERIFY(composerJob.exec());
    QCOMPARE(composerJob.resultMessages().size(), 1);
    const KMime::Message::Ptr message = composerJob.resultMessages().constFirst();
    QCOMPARE(message->to()->displayNames().size(), 1);
    QCOMPARE(message->to()->displayNames().first().toUtf8(), displayName.toUtf8());
}

void ComposerTest::fillComposerData(ComposerJob *composerJob)
{
    composerJob->infoPart()->setFrom(QStringLiteral("me@me.me"));
    composerJob->infoPart()->setTo(QStringList(QStringLiteral("you@you.you")));
    composerJob->textPart()->setWrappedPlainText(QStringLiteral("All happy families are alike; each unhappy family is unhappy in its own way."));
}

#include "moc_composertest.cpp"
