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

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/InfoPart>
#include <MessageComposer/TextPart>
using namespace MessageComposer;

#include <MessageCore/AttachmentPart>
using MessageCore::AttachmentPart;

QTEST_MAIN(ComposerTest)

void ComposerTest::testAttachments()
{
    auto composer = new Composer;
    fillComposerData(composer);
    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    composer->addAttachmentPart(attachment);

    QVERIFY(composer->exec());
    QCOMPARE(composer->resultMessages().size(), 1);
    KMime::Message::Ptr message = composer->resultMessages().constFirst();
    delete composer;
    composer = nullptr;

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
    auto composer = new Composer;
    fillComposerData(composer);
    AttachmentPart::Ptr attachment = AttachmentPart::Ptr(new AttachmentPart);
    attachment->setData("abc");
    attachment->setMimeType("x-some/x-type");
    composer->addAttachmentPart(attachment);

    // This tests if autosave in crash mode works without invoking an event loop, since using an
    // event loop in the crash handler would be a pretty bad idea
    composer->setAutoSave(true);
    composer->start();
    QVERIFY(composer->finished());
    QCOMPARE(composer->resultMessages().size(), 1);
    delete composer;
    composer = nullptr;
}

void ComposerTest::testNonAsciiHeaders()
{
    Composer composer;
    fillComposerData(&composer);

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

    composer.infoPart()->setFrom(from);
    composer.infoPart()->setTo(QStringList() << to);
    composer.infoPart()->setCc(QStringList() << cc);
    composer.infoPart()->setBcc(QStringList() << bcc);
    composer.infoPart()->setReplyTo(replyto);

    QVERIFY(composer.exec());
    QCOMPARE(composer.resultMessages().size(), 1);
    const KMime::Message::Ptr message = composer.resultMessages().constFirst();
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
    auto composer = new Composer;
    fillComposerData(composer);
    composer->infoPart()->setTo(QStringList() << (displayName + QLatin1StringView(" <") + mailbox + QLatin1StringView(">")));
    QVERIFY(composer->exec());
    QCOMPARE(composer->resultMessages().size(), 1);
    const KMime::Message::Ptr message = composer->resultMessages().constFirst();
    QCOMPARE(message->to()->displayNames().size(), 1);
    QCOMPARE(message->to()->displayNames().first().toUtf8(), displayName.toUtf8());
    delete composer;
    composer = nullptr;
}

void ComposerTest::fillComposerData(Composer *composer)
{
    composer->infoPart()->setFrom(QStringLiteral("me@me.me"));
    composer->infoPart()->setTo(QStringList(QStringLiteral("you@you.you")));
    composer->textPart()->setWrappedPlainText(QStringLiteral("All happy families are alike; each unhappy family is unhappy in its own way."));
}

#include "moc_composertest.cpp"
