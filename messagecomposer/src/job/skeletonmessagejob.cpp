/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/skeletonmessagejob.h"
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
    SkeletonMessageJobPrivate(SkeletonMessageJob *qq)
        : JobBasePrivate(qq)
    {
    }

    void doStart(); // slot

    InfoPart *infoPart = nullptr;
    GlobalPart *globalPart = nullptr;
    KMime::Message *message = nullptr;

    Q_DECLARE_PUBLIC(SkeletonMessageJob)
};

void SkeletonMessageJobPrivate::doStart()
{
    Q_Q(SkeletonMessageJob);

    Q_ASSERT(infoPart);
    Q_ASSERT(message == nullptr);
    message = new KMime::Message;

    // From:
    {
        auto from = new KMime::Headers::From;
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(infoPart->from()));
        from->fromUnicodeString(QString::fromLatin1(address.as7BitString("utf-8")));
        message->setHeader(from);
    }

    // To:
    {
        auto to = new KMime::Headers::To;
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
        message->setHeader(to);
    }

    // Reply To:
    if (!infoPart->replyTo().isEmpty()) {
        auto replyTo = new KMime::Headers::ReplyTo;
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
        message->setHeader(replyTo);
    }

    // Cc:
    {
        auto cc = new KMime::Headers::Cc;
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
        message->setHeader(cc);
    }

    // Bcc:
    {
        auto bcc = new KMime::Headers::Bcc;
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
        message->setHeader(bcc);
    }

    // Subject:
    {
        auto subject = new KMime::Headers::Subject;
        subject->fromUnicodeString(infoPart->subject());
        // TODO should we be more specific about the charset?
        message->setHeader(subject);
    }

    // Date:
    {
        auto date = new KMime::Headers::Date;
        date->setDateTime(QDateTime::currentDateTime());
        message->setHeader(date);
    }

    // Fcc:
    if (!infoPart->fcc().isEmpty()) {
        auto header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(infoPart->fcc());
        message->setHeader(header);
    }

    // Transport:
    if (infoPart->transportId() > -1) {
        auto header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(QString::number(infoPart->transportId()));
        message->setHeader(header);
    }

    // Message-ID
    {
        auto messageId = new KMime::Headers::MessageID();
        QByteArray fqdn;
        if (MessageComposer::MessageComposerSettings::self()->useCustomMessageIdSuffix()) {
            fqdn = QUrl::toAce(MessageComposer::MessageComposerSettings::self()->customMsgIDSuffix());
        }
        if (fqdn.isEmpty()) {
            fqdn = QUrl::toAce(QHostInfo::localHostName());
        }
        if (fqdn.isEmpty()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Unable to generate a Message-ID, falling back to 'localhost.localdomain'.";
            fqdn = "local.domain";
        }
        messageId->generate(fqdn);
        message->setHeader(messageId);
    }
    // Extras

    const KMime::Headers::Base::List extraHeaders = infoPart->extraHeaders();
    for (KMime::Headers::Base *extra : extraHeaders) {
        const QByteArray headerType(extra->type());
        auto copyHeader = KMime::Headers::createHeader(headerType);
        if (!copyHeader) {
            copyHeader = new KMime::Headers::Generic(headerType.constData(), headerType.size());
        }
        copyHeader->from7BitString(extra->as7BitString(false));
        message->setHeader(copyHeader);
    }

    // Request Delivery Confirmation
    {
        if (globalPart->requestDeleveryConfirmation()) {
            // TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            auto requestDeleveryConfirmation = new KMime::Headers::Generic("Return-Receipt-To");
            requestDeleveryConfirmation->fromUnicodeString(addr);
            message->setHeader(requestDeleveryConfirmation);
        }
    }

    // MDN
    {
        if (globalPart->MDNRequested()) {
            // TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            auto mdn = new KMime::Headers::Generic("Disposition-Notification-To");
            mdn->fromUnicodeString(addr);
            message->setHeader(mdn);
        }
    }

    // Urgent header
    if (infoPart->urgent()) {
        auto urg1 = new KMime::Headers::Generic("X-PRIORITY");
        urg1->fromUnicodeString(QStringLiteral("2 (High)"));
        auto urg2 = new KMime::Headers::Generic("Priority");
        urg2->fromUnicodeString(QStringLiteral("urgent"));
        message->setHeader(urg1);
        message->setHeader(urg2);
    }

    // In-Reply-To
    if (!infoPart->inReplyTo().isEmpty()) {
        auto header = new KMime::Headers::InReplyTo;
        header->fromUnicodeString(infoPart->inReplyTo());
        message->setHeader(header);
    }

    // References
    if (!infoPart->references().isEmpty()) {
        auto header = new KMime::Headers::References;
        header->fromUnicodeString(infoPart->references());
        message->setHeader(header);
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

KMime::Message *SkeletonMessageJob::message() const
{
    Q_D(const SkeletonMessageJob);
    return d->message;
}

void SkeletonMessageJob::start()
{
    Q_D(SkeletonMessageJob);
    d->doStart();
}

#include "moc_skeletonmessagejob.cpp"
