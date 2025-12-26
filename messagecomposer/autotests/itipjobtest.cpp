/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "itipjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "qtest_messagecomposer.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
#include <KMime/Headers>

#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/ItipJob>
#include <MessageComposer/ItipPart>

using namespace KMime;
using namespace MessageComposer;
using namespace MessageCore;

QTEST_MAIN(ItipJobTest)

static QString testItip = QStringLiteral(R"(
BEGIN:VCALENDAR
CALSCALE:GREGORIAN
METHOD:REQUEST
BEGIN:VEVENT
CREATED:20230508T143456Z
ORGANIZER;CN=Konqi:MAILTO:konqi@example.com
ATTENDEE;CN=Kate;RSVP=TRUE;ROLE=REQ-PARTICIPANT:MAILTO:kate@example.com
CREATED:20230508T143456Z
UID:KOrganizer-1673850046.1067
SUMMARY:Krypto Party
DTSTART;VALUE=DATE:20230520
DTEND;VALUE=DATE:20230520
END:VEVENT
END:VCALENDAR)");

static QString testItipMessage = u"Hi all, let's do some crypto partying!"_s;

void ItipJobTest::testInvitationWithAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(false);
    part->setInvitation(testItip);
    part->setInvitationBody(testItipMessage);

    ComposerJob composerJob;
    ItipJob job(part.get(), &composerJob);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    const auto content = job.takeContent();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "multipart/mixed");
    const auto subparts = content->contents();
    QCOMPARE(subparts.size(), 2);
    const auto msgPart = subparts[0];
    QCOMPARE(msgPart->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "text/plain");
    QCOMPARE(msgPart->contentDisposition(KMime::CreatePolicy::DontCreate)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(msgPart->decodedText(), testItipMessage);

    const auto itipPart = subparts[1];
    QCOMPARE(itipPart->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "text/calendar");
    QCOMPARE(itipPart->contentType(KMime::CreatePolicy::DontCreate)->name(), u"cal.ics"_s);
    QCOMPARE(itipPart->contentType(KMime::CreatePolicy::DontCreate)->parameter("method"), u"request"_s);
    QCOMPARE(itipPart->contentType(KMime::CreatePolicy::DontCreate)->charset(), "utf-8");
    QCOMPARE(itipPart->contentDisposition(KMime::CreatePolicy::DontCreate)->disposition(), KMime::Headers::CDattachment);
    QCOMPARE(itipPart->decodedText(), testItip);
}

void ItipJobTest::testInvitationWithoutAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(false);
    part->setInvitationBody(testItipMessage);

    ComposerJob composerJob;
    ItipJob job(part.get(), &composerJob);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    const auto content = job.takeContent();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "text/plain");
    QCOMPARE(content->contentDisposition(KMime::CreatePolicy::DontCreate)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(content->decodedText(), testItipMessage);
}

void ItipJobTest::testOutlookInvitationWithAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(true);
    part->setInvitation(testItip);
    part->setInvitationBody(testItipMessage);

    ComposerJob composerJob;
    ItipJob job(part.get(), &composerJob);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    const auto content = job.takeContent();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "text/calendar");
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->name(), u"cal.ics"_s);
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->parameter("method"), u"request"_s);
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->charset(), "utf-8");
    QCOMPARE(content->contentDisposition(KMime::CreatePolicy::DontCreate)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(content->decodedText(), testItip);
}

void ItipJobTest::testOutlookInvitationWithoutAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(true);
    part->setInvitationBody(testItipMessage);

    ComposerJob composerJob;
    ItipJob job(part.get(), &composerJob);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    const auto content = job.takeContent();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->mimeType(), "text/calendar");
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->name(), u"cal.ics"_s);
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->parameter("method"), u"request"_s);
    QCOMPARE(content->contentType(KMime::CreatePolicy::DontCreate)->charset(), "utf-8");
    QVERIFY(content->decodedText().isEmpty());
}

#include "moc_itipjobtest.cpp"
