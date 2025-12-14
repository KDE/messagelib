/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "composertest.h"
using namespace Qt::Literals::StringLiterals;

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
    std::shared_ptr<KMime::Message> message = composerJob.resultMessages().constFirst();

    // multipart/mixed
    {
        QVERIFY(message->contentType(KMime::CreatePolicy::DontCreate));
        QCOMPARE(message->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), QByteArray("multipart/mixed"));
        QCOMPARE(message->contents().count(), 2);
        // text/plain
        {
            Content *plain = message->contents().at(0);
            QVERIFY(plain->contentType(KMime::CreatePolicy::DontCreate));
            QCOMPARE(plain->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), QByteArray("text/plain"));
        }
        // x-some/x-type (attachment)
        {
            Content *plain = message->contents().at(1);
            QVERIFY(plain->contentType(KMime::CreatePolicy::DontCreate));
            QCOMPARE(plain->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), QByteArray("x-some/x-type"));
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

    const QString mailbox = u" <bla@example.com>"_s;
    const QString fromDisplayName = u"Hellö"_s;
    const QString toDisplayName = u"æſłĸð"_s;
    const QString ccDisplayName = u"Вася"_s;
    const QString bccDisplayName = u"ĸłſðđøþĸµ»«„¢þµ¢”«ł„·ĸ”"_s;
    const QString replyToDisplayName = u"æĸſłð˝đВасяðæĸđ"_s;
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
    const std::shared_ptr<KMime::Message> message = composerJob.resultMessages().constFirst();
    message->assemble();
    message->parse();
    QCOMPARE(message->bcc(KMime::CreatePolicy::DontCreate)->displayNames().size(), 1);
    QCOMPARE(message->to(KMime::CreatePolicy::DontCreate)->displayNames().size(), 1);
    QCOMPARE(message->cc(KMime::CreatePolicy::DontCreate)->displayNames().size(), 1);
    QCOMPARE(message->from(KMime::CreatePolicy::DontCreate)->displayNames().size(), 1);
    QCOMPARE(message->replyTo(KMime::CreatePolicy::DontCreate)->displayNames().size(), 1);
    QCOMPARE(message->from(KMime::CreatePolicy::DontCreate)->displayNames().constFirst(), fromDisplayName);
    QCOMPARE(message->to(KMime::CreatePolicy::DontCreate)->displayNames().constFirst(), toDisplayName);
    QCOMPARE(message->cc(KMime::CreatePolicy::DontCreate)->displayNames().constFirst(), ccDisplayName);
    QCOMPARE(message->bcc(KMime::CreatePolicy::DontCreate)->displayNames().constFirst(), bccDisplayName);
    QCOMPARE(message->replyTo(KMime::CreatePolicy::DontCreate)->displayNames().constFirst(), replyToDisplayName);
    message->clear();
}

void ComposerTest::testBug271192()
{
    const QString displayName = u"Интернет-компания example"_s;
    const QString mailbox = u"example@example.com"_s;
    ComposerJob composerJob;
    fillComposerData(&composerJob);
    composerJob.infoPart()->setTo(QStringList() << (displayName + QLatin1StringView(" <") + mailbox + QLatin1StringView(">")));
    QVERIFY(composerJob.exec());
    QCOMPARE(composerJob.resultMessages().size(), 1);
    const std::shared_ptr<KMime::Message> message = composerJob.resultMessages().constFirst();
    QCOMPARE(message->to()->displayNames().size(), 1);
    QCOMPARE(message->to()->displayNames().first().toUtf8(), displayName.toUtf8());
}

void ComposerTest::fillComposerData(ComposerJob *composerJob)
{
    composerJob->infoPart()->setFrom(u"me@me.me"_s);
    composerJob->infoPart()->setTo(QStringList(u"you@you.you"_s));
    composerJob->textPart()->setWrappedPlainText(u"All happy families are alike; each unhappy family is unhappy in its own way."_s);
}

#include "moc_composertest.cpp"
