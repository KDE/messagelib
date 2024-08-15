/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "itipjobtest.h"
#include "qtest_messagecomposer.h"

#include <QDebug>
#include <QTest>

#include <KMime/Content>
#include <KMime/Headers>
using namespace KMime;

#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/ItipJob>
#include <MessageComposer/ItipPart>

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

static QString testItipMessage = QStringLiteral("Hi all, let's do some crypto partying!");

void ItipJobTest::testInvitationWithAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(false);
    part->setInvitation(testItip);
    part->setInvitationBody(testItipMessage);

    Composer composer;
    ItipJob job(part.get(), &composer);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    auto *content = job.content();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(false)->mimeType(), "multipart/mixed");
    const auto subparts = content->contents();
    QCOMPARE(subparts.size(), 2);
    const auto msgPart = subparts[0];
    QCOMPARE(msgPart->contentType(false)->mimeType(), "text/plain");
    QCOMPARE(msgPart->contentDisposition(false)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(msgPart->decodedText(), testItipMessage);

    const auto itipPart = subparts[1];
    QCOMPARE(itipPart->contentType(false)->mimeType(), "text/calendar");
    QCOMPARE(itipPart->contentType(false)->name(), QStringLiteral("cal.ics"));
    QCOMPARE(itipPart->contentType(false)->parameter("method"), QStringLiteral("request"));
    QCOMPARE(itipPart->contentType(false)->charset(), "utf-8");
    QCOMPARE(itipPart->contentDisposition(false)->disposition(), KMime::Headers::CDattachment);
    QCOMPARE(itipPart->decodedText(), testItip);
}

void ItipJobTest::testInvitationWithoutAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(false);
    part->setInvitationBody(testItipMessage);

    Composer composer;
    ItipJob job(part.get(), &composer);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    auto *content = job.content();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(false)->mimeType(), "text/plain");
    QCOMPARE(content->contentDisposition(false)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(content->decodedText(), testItipMessage);
}

void ItipJobTest::testOutlookInvitationWithAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(true);
    part->setInvitation(testItip);
    part->setInvitationBody(testItipMessage);

    Composer composer;
    ItipJob job(part.get(), &composer);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    auto *content = job.content();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(false)->mimeType(), "text/calendar");
    QCOMPARE(content->contentType(false)->name(), QStringLiteral("cal.ics"));
    QCOMPARE(content->contentType(false)->parameter("method"), QStringLiteral("request"));
    QCOMPARE(content->contentType(false)->charset(), "utf-8");
    QCOMPARE(content->contentDisposition(false)->disposition(), KMime::Headers::CDinline);
    QCOMPARE(content->decodedText(), testItip);
}

void ItipJobTest::testOutlookInvitationWithoutAttachment()
{
    auto part = std::make_unique<ItipPart>();
    part->setOutlookConformInvitation(true);
    part->setInvitationBody(testItipMessage);

    Composer composer;
    ItipJob job(part.get(), &composer);
    job.setAutoDelete(false);
    QVERIFY(job.exec());

    auto *content = job.content();
    content->assemble();
    QVERIFY(content);

    QCOMPARE(content->contentType(false)->mimeType(), "text/calendar");
    QCOMPARE(content->contentType(false)->name(), QStringLiteral("cal.ics"));
    QCOMPARE(content->contentType(false)->parameter("method"), QStringLiteral("request"));
    QCOMPARE(content->contentType(false)->charset(), "utf-8");
    QVERIFY(content->decodedText().isEmpty());
}

#include "moc_itipjobtest.cpp"
