/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/skeletonmessagejob.h"
using namespace Qt::Literals::StringLiterals;

#include "settings/messagecomposersettings.h"

#include "job/jobbase_p.h"
#include "part/globalpart.h"
#include "part/infopart.h"

#include <QHostInfo>
#include <QUrl>

#include "messagecomposer_debug.h"

#include <KEmailAddress>
#include <KMime/Message>

using namespace MessageComposer;

class MessageComposer::SkeletonMessageJobPrivate : public JobBasePrivate
{
public:
    explicit SkeletonMessageJobPrivate(SkeletonMessageJob *qq)
        : JobBasePrivate(qq)
    {
    }

    void doStart(); // slot

    InfoPart *infoPart = nullptr;
    GlobalPart *globalPart = nullptr;
    std::unique_ptr<KMime::Message> message;

    Q_DECLARE_PUBLIC(SkeletonMessageJob)
};

void SkeletonMessageJobPrivate::doStart()
{
    Q_Q(SkeletonMessageJob);

    Q_ASSERT(infoPart);
    Q_ASSERT(!message);
    message = std::make_unique<KMime::Message>();

    KMime::Types::Mailbox fromAddress;
    fromAddress.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(infoPart->from()));

    // From:
    {
        auto from = std::make_unique<KMime::Headers::From>();
        from->fromUnicodeString(QString::fromLatin1(fromAddress.as7BitString("utf-8")));
        message->setHeader(std::move(from));
    }

    // To:
    {
        auto to = std::make_unique<KMime::Headers::To>();
        QByteArray sTo;
        const QStringList lstTo = infoPart->to();
        for (const QString &a : lstTo) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sTo.isEmpty()) {
                sTo.append(",");
            }
            sTo.append(address.as7BitString("utf-8"));
        }
        to->fromUnicodeString(QString::fromLatin1(sTo));
        message->setHeader(std::move(to));
    }

    // Reply To:
    if (!infoPart->replyTo().isEmpty()) {
        auto replyTo = std::make_unique<KMime::Headers::ReplyTo>();
        const QStringList lstReplyTo = infoPart->replyTo();
        QByteArray sReplyTo;
        for (const QString &a : lstReplyTo) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sReplyTo.isEmpty()) {
                sReplyTo.append(",");
            }
            sReplyTo.append(address.as7BitString("utf-8"));
        }
        replyTo->fromUnicodeString(QString::fromLatin1(sReplyTo));
        message->setHeader(std::move(replyTo));
    }

    // Cc:
    {
        auto cc = std::make_unique<KMime::Headers::Cc>();
        QByteArray sCc;
        const QStringList lstCc = infoPart->cc();
        for (const QString &a : lstCc) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sCc.isEmpty()) {
                sCc.append(",");
            }
            sCc.append(address.as7BitString("utf-8"));
        }
        cc->fromUnicodeString(QString::fromLatin1(sCc));
        message->setHeader(std::move(cc));
    }

    // Bcc:
    {
        auto bcc = std::make_unique<KMime::Headers::Bcc>();
        QByteArray sBcc;
        const QStringList lstBcc = infoPart->bcc();
        for (const QString &a : lstBcc) {
            KMime::Types::Mailbox address;
            address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(a));
            if (!sBcc.isEmpty()) {
                sBcc.append(",");
            }
            sBcc.append(address.as7BitString("utf-8"));
        }
        bcc->fromUnicodeString(QString::fromLatin1(sBcc));
        message->setHeader(std::move(bcc));
    }

    // Subject:
    {
        auto subject = std::make_unique<KMime::Headers::Subject>();
        subject->fromUnicodeString(infoPart->subject());
        // TODO should we be more specific about the charset?
        message->setHeader(std::move(subject));
    }

    // Date:
    {
        auto date = std::make_unique<KMime::Headers::Date>();
        date->setDateTime(QDateTime::currentDateTime());
        message->setHeader(std::move(date));
    }

    // Fcc:
    if (!infoPart->fcc().isEmpty()) {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Fcc");
        header->fromUnicodeString(infoPart->fcc());
        message->setHeader(std::move(header));
    }

    // Transport:
    if (infoPart->transportId() > -1) {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Transport");
        header->fromUnicodeString(QString::number(infoPart->transportId()));
        message->setHeader(std::move(header));
    }

    // Message-ID
    {
        const auto fromParts = infoPart->from();
        auto messageId = std::make_unique<KMime::Headers::MessageID>();
        messageId->generate(fromAddress.addrSpec().domain.toUtf8());
        message->setHeader(std::move(messageId));
    }
    // Extras

    const QList<KMime::Headers::Base *> extraHeaders = infoPart->extraHeaders();
    for (KMime::Headers::Base *extra : extraHeaders) {
        const QByteArray headerType(extra->type());
        auto copyHeader = KMime::Headers::createHeader(headerType);
        if (!copyHeader) {
            copyHeader = std::make_unique<KMime::Headers::Generic>(headerType.constData(), headerType.size());
        }
        copyHeader->from7BitString(extra->as7BitString());
        message->setHeader(std::move(copyHeader));
    }

    // Request Delivery Confirmation
    {
        if (globalPart->requestDeleveryConfirmation()) {
            // TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            auto requestDeleveryConfirmation = std::make_unique<KMime::Headers::Generic>("Return-Receipt-To");
            requestDeleveryConfirmation->fromUnicodeString(addr);
            message->setHeader(std::move(requestDeleveryConfirmation));
        }
    }

    // MDN
    {
        if (globalPart->MDNRequested()) {
            // TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            auto mdn = std::make_unique<KMime::Headers::Generic>("Disposition-Notification-To");
            mdn->fromUnicodeString(addr);
            message->setHeader(std::move(mdn));
        }
    }

    // Urgent header
    if (infoPart->urgent()) {
        auto urg1 = std::make_unique<KMime::Headers::Generic>("X-PRIORITY");
        urg1->fromUnicodeString(u"2 (High)"_s);
        auto urg2 = std::make_unique<KMime::Headers::Generic>("Priority");
        urg2->fromUnicodeString(u"urgent"_s);
        message->setHeader(std::move(urg1));
        message->setHeader(std::move(urg2));
    }

    // In-Reply-To
    if (!infoPart->inReplyTo().isEmpty()) {
        auto header = std::make_unique<KMime::Headers::InReplyTo>();
        header->fromUnicodeString(infoPart->inReplyTo());
        message->setHeader(std::move(header));
    }

    // References
    if (!infoPart->references().isEmpty()) {
        auto header = std::make_unique<KMime::Headers::References>();
        header->fromUnicodeString(infoPart->references());
        message->setHeader(std::move(header));
    }

    q->emitResult(); // Success.
}

SkeletonMessageJob::SkeletonMessageJob(InfoPart *infoPart, GlobalPart *globalPart, QObject *parent)
    : JobBase(*new SkeletonMessageJobPrivate(this), parent)
{
    Q_D(SkeletonMessageJob);
    d->infoPart = infoPart;
    d->globalPart = globalPart;
}

SkeletonMessageJob::~SkeletonMessageJob() = default;

InfoPart *SkeletonMessageJob::infoPart() const
{
    Q_D(const SkeletonMessageJob);
    return d->infoPart;
}

void SkeletonMessageJob::setInfoPart(InfoPart *part)
{
    Q_D(SkeletonMessageJob);
    d->infoPart = part;
}

GlobalPart *SkeletonMessageJob::globalPart() const
{
    Q_D(const SkeletonMessageJob);
    return d->globalPart;
}

void SkeletonMessageJob::setGlobalPart(GlobalPart *part)
{
    Q_D(SkeletonMessageJob);
    d->globalPart = part;
}

std::unique_ptr<KMime::Message> &&SkeletonMessageJob::takeMessage()
{
    Q_D(SkeletonMessageJob);
    return std::move(d->message);
}

void SkeletonMessageJob::start()
{
    Q_D(SkeletonMessageJob);
    d->doStart();
}

#include "moc_skeletonmessagejob.cpp"
