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
        auto from = std::unique_ptr<KMime::Headers::From>(new KMime::Headers::From);
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(infoPart->from()));
        from->fromUnicodeString(QString::fromLatin1(address.as7BitString("utf-8")));
        message->setHeader(std::move(from));
    }

    // To:
    {
        auto to = std::unique_ptr<KMime::Headers::To>(new KMime::Headers::To);
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
        auto replyTo = std::unique_ptr<KMime::Headers::ReplyTo>(new KMime::Headers::ReplyTo);
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
        auto cc = std::unique_ptr<KMime::Headers::Cc>(new KMime::Headers::Cc);
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
        auto bcc = std::unique_ptr<KMime::Headers::Bcc>(new KMime::Headers::Bcc);
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
        auto subject = std::unique_ptr<KMime::Headers::Subject>(new KMime::Headers::Subject);
        subject->fromUnicodeString(infoPart->subject());
        // TODO should we be more specific about the charset?
        message->setHeader(std::move(subject));
    }

    // Date:
    {
        auto date = std::unique_ptr<KMime::Headers::Date>(new KMime::Headers::Date);
        date->setDateTime(QDateTime::currentDateTime());
        message->setHeader(std::move(date));
    }

    // Fcc:
    if (!infoPart->fcc().isEmpty()) {
        auto header = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("X-KMail-Fcc"));
        header->fromUnicodeString(infoPart->fcc());
        message->setHeader(std::move(header));
    }

    // Transport:
    if (infoPart->transportId() > -1) {
        auto header = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("X-KMail-Transport"));
        header->fromUnicodeString(QString::number(infoPart->transportId()));
        message->setHeader(std::move(header));
    }

    // Message-ID
    {
        auto messageId = std::unique_ptr<KMime::Headers::MessageID>(new KMime::Headers::MessageID());
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
        message->setHeader(std::move(messageId));
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
            auto requestDeleveryConfirmation = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("Return-Receipt-To"));
            requestDeleveryConfirmation->fromUnicodeString(addr);
            message->setHeader(std::move(requestDeleveryConfirmation));
        }
    }

    // MDN
    {
        if (globalPart->MDNRequested()) {
            // TODO fix me multi address
            const QString addr = infoPart->replyTo().isEmpty() ? infoPart->from() : infoPart->replyTo().at(0);
            auto mdn = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("Disposition-Notification-To"));
            mdn->fromUnicodeString(addr);
            message->setHeader(std::move(mdn));
        }
    }

    // Urgent header
    if (infoPart->urgent()) {
        auto urg1 = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("X-PRIORITY"));
        urg1->fromUnicodeString(u"2 (High)"_s);
        auto urg2 = std::unique_ptr<KMime::Headers::Generic>(new KMime::Headers::Generic("Priority"));
        urg2->fromUnicodeString(u"urgent"_s);
        message->setHeader(std::move(urg1));
        message->setHeader(std::move(urg2));
    }

    // In-Reply-To
    if (!infoPart->inReplyTo().isEmpty()) {
        auto header = std::unique_ptr<KMime::Headers::InReplyTo>(new KMime::Headers::InReplyTo);
        header->fromUnicodeString(infoPart->inReplyTo());
        message->setHeader(std::move(header));
    }

    // References
    if (!infoPart->references().isEmpty()) {
        auto header = std::unique_ptr<KMime::Headers::References>(new KMime::Headers::References);
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
