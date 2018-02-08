/*
  Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>
  Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "messagefactoryng.h"

#include "settings/messagecomposersettings.h"
#include "messagefactoryforwardjob.h"
#include "messagefactoryreplyjob.h"
#include "MessageComposer/Util"

#include <AkonadiCore/item.h>
#ifndef QT_NO_CURSOR
#include <Libkdepim/KCursorSaver>
#endif
#include <KIdentityManagement/kidentitymanagement/identitymanager.h>
#include <KIdentityManagement/kidentitymanagement/identity.h>

#include <Akonadi/KMime/MessageStatus>
#include <kmime/kmime_dateformatter.h>
#include <KEmailAddress>
#include <MessageCore/MailingList>
#include <MessageCore/StringUtil>
#include "helper/messagehelper.h"
#include <KLocalizedString>
#include "messagecomposer_debug.h"
#include <kcharsets.h>
#include <QTextCodec>
#include <KCharsets>

using namespace MessageComposer;

namespace KMime {
namespace Types {
static bool operator==(const KMime::Types::Mailbox &left, const KMime::Types::Mailbox &right)
{
    return left.addrSpec().asString() == right.addrSpec().asString();
}
}
}

/**
 * Strips all the user's addresses from an address list. This is used
 * when replying.
 */
static KMime::Types::Mailbox::List stripMyAddressesFromAddressList(const KMime::Types::Mailbox::List &list, const KIdentityManagement::IdentityManager *manager)
{
    KMime::Types::Mailbox::List addresses(list);
    for (KMime::Types::Mailbox::List::Iterator it = addresses.begin(); it != addresses.end();) {
        if (manager->thatIsMe(it->prettyAddress())) {
            it = addresses.erase(it);
        } else {
            ++it;
        }
    }

    return addresses;
}

MessageFactoryNG::MessageFactoryNG(const KMime::Message::Ptr &origMsg, Akonadi::Item::Id id, const Akonadi::Collection &col, QObject *parent)
    : QObject(parent)
    , m_identityManager(nullptr)
    , m_origMsg(origMsg)
    , m_folderId(0)
    , m_parentFolderId(0)
    , m_collection(col)
    , m_replyStrategy(MessageComposer::ReplySmart)
    , m_quote(true)
    , m_id(id)
{
}

MessageFactoryNG::~MessageFactoryNG()
{
}

void MessageFactoryNG::slotCreateReplyDone(const KMime::Message::Ptr &msg, bool replyAll)
{
    applyCharset(msg);

    MessageComposer::Util::addLinkInformation(msg, m_id, Akonadi::MessageStatus::statusReplied());
    if (m_parentFolderId > 0) {
        KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(QString::number(m_parentFolderId), "utf-8");
        msg->setHeader(header);
    }

    if (auto hrd = m_origMsg->headerByType("X-KMail-EncryptActionEnabled")) {
        if (hrd->as7BitString(false).contains("true")) {
            auto header = new KMime::Headers::Generic("X-KMail-EncryptActionEnabled");
            header->fromUnicodeString(QStringLiteral("true"), "utf-8");
            msg->setHeader(header);
        }
    }
    msg->assemble();

    MessageReply reply;
    reply.msg = msg;
    reply.replyAll = replyAll;
    Q_EMIT createReplyDone(reply);
}

void MessageFactoryNG::createReplyAsync()
{
    KMime::Message::Ptr msg(new KMime::Message);
    QByteArray refStr;
    bool replyAll = true;
    KMime::Types::Mailbox::List toList;
    KMime::Types::Mailbox::List replyToList;

    const uint originalIdentity = identityUoid(m_origMsg);
    MessageHelper::initFromMessage(msg, m_origMsg, m_identityManager, originalIdentity);
    replyToList = m_origMsg->replyTo()->mailboxes();

    msg->contentType()->setCharset("utf-8");

    if (auto hdr = m_origMsg->headerByType("List-Post")) {
        const QString hdrListPost = hdr->asUnicodeString();
        if (hdrListPost.contains(QLatin1String("mailto:"), Qt::CaseInsensitive)) {
            QRegExp rx(QStringLiteral("<mailto:([^@>]+)@([^>]+)>"), Qt::CaseInsensitive);
            if (rx.indexIn(hdrListPost, 0) != -1) {   // matched
                KMime::Types::Mailbox mailbox;
                mailbox.fromUnicodeString(rx.cap(1) + QLatin1Char('@') + rx.cap(2));
                m_mailingListAddresses << mailbox;
            }
        }
    }

    switch (m_replyStrategy) {
    case MessageComposer::ReplySmart:
    {
        if (auto hdr = m_origMsg->headerByType("Mail-Followup-To")) {
            toList << KMime::Types::Mailbox::listFrom7BitString(hdr->as7BitString(false));
        } else if (!replyToList.isEmpty()) {
            toList = replyToList;
            // use the ReplyAll template only when it's a reply to a mailing list
            if (m_mailingListAddresses.isEmpty()) {
                replyAll = false;
            }
        } else if (!m_mailingListAddresses.isEmpty()) {
            toList = (KMime::Types::Mailbox::List() << m_mailingListAddresses.at(0));
        } else {
            // doesn't seem to be a mailing list, reply to From: address
            toList = m_origMsg->from()->mailboxes();

            if (m_identityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(toList))) {
                // sender seems to be one of our own identities, so we assume that this
                // is a reply to a "sent" mail where the users wants to add additional
                // information for the recipient.
                toList = m_origMsg->to()->mailboxes();
            }

            replyAll = false;
        }
        // strip all my addresses from the list of recipients
        const KMime::Types::Mailbox::List recipients = toList;

        toList = stripMyAddressesFromAddressList(recipients, m_identityManager);

        // ... unless the list contains only my addresses (reply to self)
        if (toList.isEmpty() && !recipients.isEmpty()) {
            toList << recipients.first();
        }
        break;
    }
    case MessageComposer::ReplyList:
    {
        if (auto hdr = m_origMsg->headerByType("Mail-Followup-To")) {
            KMime::Types::Mailbox mailbox;
            mailbox.from7BitString(hdr->as7BitString(false));
            toList << mailbox;
        } else if (!m_mailingListAddresses.isEmpty()) {
            toList << m_mailingListAddresses[ 0 ];
        } else if (!replyToList.isEmpty()) {
            // assume a Reply-To header mangling mailing list
            toList = replyToList;
        }

        // strip all my addresses from the list of recipients
        const KMime::Types::Mailbox::List recipients = toList;

        toList = stripMyAddressesFromAddressList(recipients, m_identityManager);
        break;
    }
    case MessageComposer::ReplyAll:
    {
        KMime::Types::Mailbox::List recipients;
        KMime::Types::Mailbox::List ccRecipients;

        QString sender;
        if (auto hrd = m_origMsg->sender(false)) {
            sender = hrd->asUnicodeString();
        }
        // add addresses from the Reply-To header to the list of recipients
        if (!replyToList.isEmpty()) {
            recipients = replyToList;

            // strip all possible mailing list addresses from the list of Reply-To addresses
            for (const KMime::Types::Mailbox &mailbox : qAsConst(m_mailingListAddresses)) {
                foreach (const KMime::Types::Mailbox &recipient, recipients) { //Don't use for(...:...)
                    if (mailbox == recipient) {
                        recipients.removeAll(recipient);
                    }
                }
            }
        }
        bool stripMyAddresses = true;
        if (!m_mailingListAddresses.isEmpty()) {
            // this is a mailing list message
            if (recipients.isEmpty() && !m_origMsg->from()->asUnicodeString().isEmpty()) {
                // The sender didn't set a Reply-to address, so we add the From
                // address to the list of CC recipients.
                ccRecipients += m_origMsg->from()->mailboxes();
                qCDebug(MESSAGECOMPOSER_LOG) << "Added" << m_origMsg->from()->asUnicodeString() << "to the list of CC recipients";
            }

            // if it is a mailing list, add the posting address
            recipients.prepend(m_mailingListAddresses[ 0 ]);
        } else {
            const QString fromAddress = m_origMsg->from()->asUnicodeString();
            if (!fromAddress.isEmpty()) {
                if (!sender.isEmpty() && m_identityManager->thatIsMe(fromAddress)) {
                    // strip all my addresses from the list of recipients
                    toList = recipients;
                    toList += m_origMsg->from()->mailboxes();
                    stripMyAddresses = false;
                } else {
                    // this is a normal message
                    if (recipients.isEmpty()) {
                        // in case of replying to a normal message only then add the From
                        // address to the list of recipients if there was no Reply-to address
                        recipients += m_origMsg->from()->mailboxes();
                        qCDebug(MESSAGECOMPOSER_LOG) << "Added" << m_origMsg->from()->asUnicodeString() << "to the list of recipients";
                    }
                }
            }
        }
        if (stripMyAddresses) {
            // strip all my addresses from the list of recipients
            toList = stripMyAddressesFromAddressList(recipients, m_identityManager);
        }

        // merge To header and CC header into a list of CC recipients
        if (!m_origMsg->cc()->asUnicodeString().isEmpty() || !m_origMsg->to()->asUnicodeString().isEmpty()) {
            KMime::Types::Mailbox::List list;
            if (!m_origMsg->to()->asUnicodeString().isEmpty()) {
                list += m_origMsg->to()->mailboxes();
            }
            if (!m_origMsg->cc()->asUnicodeString().isEmpty()) {
                list += m_origMsg->cc()->mailboxes();
            }

            for (const KMime::Types::Mailbox &mailbox : qAsConst(list)) {
                if (!recipients.contains(mailbox)
                    && !ccRecipients.contains(mailbox)) {
                    ccRecipients += mailbox;
                    qCDebug(MESSAGECOMPOSER_LOG) << "Added" << mailbox.prettyAddress() << "to the list of CC recipients";
                }
            }
        }

        if (!ccRecipients.isEmpty()) {
            // strip all my addresses from the list of CC recipients
            if (stripMyAddresses) {
                ccRecipients = stripMyAddressesFromAddressList(ccRecipients, m_identityManager);
            }

            // in case of a reply to self, toList might be empty. if that's the case
            // then propagate a cc recipient to To: (if there is any).
            if (toList.isEmpty() && !ccRecipients.isEmpty()) {
                toList << ccRecipients.at(0);
                ccRecipients.pop_front();
            }

            for (const KMime::Types::Mailbox &mailbox : qAsConst(ccRecipients)) {
                msg->cc()->addAddress(mailbox);
            }
        }

        if (toList.isEmpty() && !recipients.isEmpty()) {
            // reply to self without other recipients
            toList << recipients.at(0);
        }
        break;
    }
    case MessageComposer::ReplyAuthor:
        if (!replyToList.isEmpty()) {
            KMime::Types::Mailbox::List recipients = replyToList;

            // strip the mailing list post address from the list of Reply-To
            // addresses since we want to reply in private
            for (const KMime::Types::Mailbox &mailbox : qAsConst(m_mailingListAddresses)) {
                foreach (const KMime::Types::Mailbox &recipient, recipients) { //Don't use for(...:...)
                    if (mailbox == recipient) {
                        recipients.removeAll(recipient);
                    }
                }
            }

            if (!recipients.isEmpty()) {
                toList = recipients;
            } else {
                // there was only the mailing list post address in the Reply-To header,
                // so use the From address instead
                toList = m_origMsg->from()->mailboxes();
            }
        } else if (!m_origMsg->from()->asUnicodeString().isEmpty()) {
            toList = m_origMsg->from()->mailboxes();
        }

        replyAll = false;
        break;
    case MessageComposer::ReplyNone:
        // the addressees will be set by the caller
        break;
    default:
        Q_UNREACHABLE();
    }

    for (const KMime::Types::Mailbox &mailbox : qAsConst(toList)) {
        msg->to()->addAddress(mailbox);
    }

    refStr = getRefStr(m_origMsg);
    if (!refStr.isEmpty()) {
        msg->references()->fromUnicodeString(QString::fromLocal8Bit(refStr), "utf-8");
    }
    //In-Reply-To = original msg-id
    msg->inReplyTo()->from7BitString(m_origMsg->messageID()->as7BitString(false));

    msg->subject()->fromUnicodeString(MessageHelper::replySubject(m_origMsg), "utf-8");

    // If the reply shouldn't be blank, apply the template to the message
    if (m_quote) {
        MessageFactoryReplyJob *job = new MessageFactoryReplyJob;
        connect(job, &MessageFactoryReplyJob::replyDone, this, &MessageFactoryNG::slotCreateReplyDone);
        job->setMsg(msg);
        job->setReplyAll(replyAll);
        job->setIdentityManager(m_identityManager);
        job->setSelection(m_selection);
        job->setTemplate(m_template);
        job->setOrigMsg(m_origMsg);
        job->setCollection(m_collection);
        job->start();
    } else {
        slotCreateReplyDone(msg, replyAll);
    }
}

void MessageFactoryNG::slotCreateForwardDone(const KMime::Message::Ptr &msg)
{
    applyCharset(msg);

    MessageComposer::Util::addLinkInformation(msg, m_id, Akonadi::MessageStatus::statusForwarded());
    msg->assemble();
    Q_EMIT createForwardDone(msg);
}

void MessageFactoryNG::createForwardAsync()
{
    KMime::Message::Ptr msg(new KMime::Message);

    // This is a non-multipart, non-text mail (e.g. text/calendar). Construct
    // a multipart/mixed mail and add the original body as an attachment.
    if (!m_origMsg->contentType()->isMultipart()
        && (!m_origMsg->contentType()->isText()
            || (m_origMsg->contentType()->isText() && m_origMsg->contentType()->subType() != "html"
                && m_origMsg->contentType()->subType() != "plain"))) {
        const uint originalIdentity = identityUoid(m_origMsg);
        MessageHelper::initFromMessage(msg, m_origMsg, m_identityManager, originalIdentity);
        msg->removeHeader<KMime::Headers::ContentType>();
        msg->removeHeader<KMime::Headers::ContentTransferEncoding>();

        msg->contentType()->setMimeType("multipart/mixed");

        //TODO: Andras: somebody should check if this is correct. :)
        // empty text part
        KMime::Content *msgPart = new KMime::Content;
        msgPart->contentType()->setMimeType("text/plain");
        msg->addContent(msgPart);

        // the old contents of the mail
        KMime::Content *secondPart = new KMime::Content;
        secondPart->contentType()->setMimeType(m_origMsg->contentType()->mimeType());
        secondPart->setBody(m_origMsg->body());
        // use the headers of the original mail
        secondPart->setHead(m_origMsg->head());
        msg->addContent(secondPart);
        msg->assemble();
    }
    // Normal message (multipart or text/plain|html)
    // Just copy the message, the template parser will do the hard work of
    // replacing the body text in TemplateParser::addProcessedBodyToMessage()
    else {
        //TODO Check if this is ok
        msg->setHead(m_origMsg->head());
        msg->setBody(m_origMsg->body());
        QString oldContentType = msg->contentType()->asUnicodeString();
        const uint originalIdentity = identityUoid(m_origMsg);
        MessageHelper::initFromMessage(msg, m_origMsg, m_identityManager, originalIdentity);

        // restore the content type, MessageHelper::initFromMessage() sets the contents type to
        // text/plain, via initHeader(), for unclear reasons
        msg->contentType()->fromUnicodeString(oldContentType, "utf-8");
        msg->assemble();
    }

    msg->subject()->fromUnicodeString(MessageHelper::forwardSubject(m_origMsg), "utf-8");
    MessageFactoryForwardJob *job = new MessageFactoryForwardJob;
    connect(job, &MessageFactoryForwardJob::forwardDone, this, &MessageFactoryNG::slotCreateForwardDone);
    job->setIdentityManager(m_identityManager);
    job->setMsg(msg);
    job->setSelection(m_selection);
    job->setTemplate(m_template);
    job->setOrigMsg(m_origMsg);
    job->setCollection(m_collection);
    job->start();
}

QPair< KMime::Message::Ptr, QList< KMime::Content * > > MessageFactoryNG::createAttachedForward(const Akonadi::Item::List &items)
{
    // create forwarded message with original message as attachment
    // remove headers that shouldn't be forwarded
    KMime::Message::Ptr msg(new KMime::Message);
    QList< KMime::Content * > attachments;

    const int numberOfItems(items.count());
    if (numberOfItems >= 2) {
        // don't respect X-KMail-Identity headers because they might differ for
        // the selected mails
        MessageHelper::initHeader(msg, m_identityManager, 0);
    } else if (numberOfItems == 1) {
        KMime::Message::Ptr firstMsg = MessageComposer::Util::message(items.first());
        const uint originalIdentity = identityUoid(firstMsg);
        MessageHelper::initFromMessage(msg, firstMsg, m_identityManager, originalIdentity);
        msg->subject()->fromUnicodeString(MessageHelper::forwardSubject(firstMsg), "utf-8");
    }

    MessageHelper::setAutomaticFields(msg, true);
#ifndef QT_NO_CURSOR
    KPIM::KCursorSaver busy(KPIM::KBusyPtr::busy());
#endif
    if (numberOfItems == 0) {
        attachments << createForwardAttachmentMessage(m_origMsg);
        MessageComposer::Util::addLinkInformation(msg, m_id, Akonadi::MessageStatus::statusForwarded());
    } else {
        // iterate through all the messages to be forwarded
        attachments.reserve(items.count());
        for (const Akonadi::Item &item : qAsConst(items)) {
            attachments << createForwardAttachmentMessage(MessageComposer::Util::message(item));
            MessageComposer::Util::addLinkInformation(msg, item.id(), Akonadi::MessageStatus::statusForwarded());
        }
    }

    applyCharset(msg);

    //msg->assemble();
    return QPair< KMime::Message::Ptr, QList< KMime::Content * > >(msg, QList< KMime::Content * >() << attachments);
}

KMime::Content *MessageFactoryNG::createForwardAttachmentMessage(const KMime::Message::Ptr &fwdMsg)
{
    // remove headers that shouldn't be forwarded
    MessageCore::StringUtil::removePrivateHeaderFields(fwdMsg);
    fwdMsg->removeHeader<KMime::Headers::Bcc>();
    fwdMsg->assemble();
    // set the part
    KMime::Content *msgPart = new KMime::Content(fwdMsg.data());
    msgPart->contentType()->setMimeType("message/rfc822");

    msgPart->contentDisposition()->setParameter(QStringLiteral("filename"), i18n("forwarded message"));
    msgPart->contentDisposition()->setDisposition(KMime::Headers::CDinline);
    msgPart->contentDescription()->fromUnicodeString(fwdMsg->from()->asUnicodeString() + QLatin1String(": ") + fwdMsg->subject()->asUnicodeString(), "utf-8");
    msgPart->setBody(fwdMsg->encodedContent());
    msgPart->assemble();

    MessageComposer::Util::addLinkInformation(fwdMsg, 0, Akonadi::MessageStatus::statusForwarded());
    return msgPart;
}

KMime::Message::Ptr MessageFactoryNG::createResend()
{
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(m_origMsg->encodedContent());
    msg->parse();
    msg->removeHeader<KMime::Headers::MessageID>();
    uint originalIdentity = identityUoid(m_origMsg);

    // Set the identity from above
    KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Identity");
    header->fromUnicodeString(QString::number(originalIdentity), "utf-8");
    msg->setHeader(header);

    // Restore the original bcc field as this is overwritten in applyIdentity
    msg->bcc(m_origMsg->bcc());
    return msg;
}

KMime::Message::Ptr MessageFactoryNG::createRedirect(const QString &toStr, const QString &ccStr, const QString &bccStr, int transportId, const QString &fcc, int identity)
{
    if (!m_origMsg) {
        return KMime::Message::Ptr();
    }

    // copy the message 1:1
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(m_origMsg->encodedContent());
    msg->parse();

    uint id = identity;
    if (identity == -1) {
        if (auto hrd = msg->headerByType("X-KMail-Identity")) {
            const QString strId = hrd->asUnicodeString().trimmed();
            if (!strId.isEmpty()) {
                id = strId.toUInt();
            }
        }
    }
    const KIdentityManagement::Identity &ident
        = m_identityManager->identityForUoidOrDefault(id);

    // X-KMail-Redirect-From: content
    const QString strByWayOf = QString::fromLocal8Bit("%1 (by way of %2 <%3>)")
                               .arg(m_origMsg->from()->asUnicodeString(), ident.fullName(), ident.primaryEmailAddress());

    // Resent-From: content
    const QString strFrom = QString::fromLocal8Bit("%1 <%2>")
                            .arg(ident.fullName(), ident.primaryEmailAddress());

    // format the current date to be used in Resent-Date:
    // FIXME: generate datetime the same way as KMime, otherwise we get inconsistency
    // in unit-tests. Unfortunatelly RFC2822Date is not enough for us, we need the
    // composition hack below
    const QDateTime dt = QDateTime::currentDateTime();
    const QString newDate = QLocale::c().toString(dt, QStringLiteral("ddd, "))
                            +dt.toString(Qt::RFC2822Date);

    // Clean up any resent headers
    msg->removeHeader("Resent-Cc");
    msg->removeHeader("Resent-Bcc");
    msg->removeHeader("Resent-Sender");
    // date, from to and id will be set anyway

    // prepend Resent-*: headers (c.f. RFC2822 3.6.6)
    QString msgIdSuffix;
    if (MessageComposer::MessageComposerSettings::useCustomMessageIdSuffix()) {
        msgIdSuffix = MessageComposer::MessageComposerSettings::customMsgIDSuffix();
    }
    KMime::Headers::Generic *header = new KMime::Headers::Generic("Resent-Message-ID");
    header->fromUnicodeString(MessageCore::StringUtil::generateMessageId(msg->sender()->asUnicodeString(), msgIdSuffix), "utf-8");
    msg->setHeader(header);

    header = new KMime::Headers::Generic("Resent-Date");
    header->fromUnicodeString(newDate, "utf-8");
    msg->setHeader(header);

    header = new KMime::Headers::Generic("Resent-From");
    header->fromUnicodeString(strFrom, "utf-8");
    msg->setHeader(header);

    if (msg->to(false)) {
        KMime::Headers::To *headerT = new KMime::Headers::To;
        headerT->fromUnicodeString(m_origMsg->to()->asUnicodeString(), "utf-8");
        msg->setHeader(headerT);
    }

    header = new KMime::Headers::Generic("Resent-To");
    header->fromUnicodeString(toStr, "utf-8");
    msg->setHeader(header);

    if (!ccStr.isEmpty()) {
        header = new KMime::Headers::Generic("Resent-Cc");
        header->fromUnicodeString(ccStr, "utf-8");
        msg->setHeader(header);
    }

    if (!bccStr.isEmpty()) {
        header = new KMime::Headers::Generic("Resent-Bcc");
        header->fromUnicodeString(bccStr, "utf-8");
        msg->setHeader(header);
    }

    header = new KMime::Headers::Generic("X-KMail-Redirect-From");
    header->fromUnicodeString(strByWayOf, "utf-8");
    msg->setHeader(header);

    if (transportId != -1) {
        header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(QString::number(transportId), "utf-8");
        msg->setHeader(header);
    }

    if (!fcc.isEmpty()) {
        header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(fcc, "utf-8");
        msg->setHeader(header);
    }

    const bool fccIsDisabled = ident.disabledFcc();
    if (fccIsDisabled) {
        KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-FccDisabled");
        header->fromUnicodeString(QStringLiteral("true"), "utf-8");
        msg->setHeader(header);
    } else {
        msg->removeHeader("X-KMail-FccDisabled");
    }

    msg->assemble();

    MessageComposer::Util::addLinkInformation(msg, m_id, Akonadi::MessageStatus::statusForwarded());
    return msg;
}

KMime::Message::Ptr MessageFactoryNG::createDeliveryReceipt()
{
    QString receiptTo;
    if (auto hrd = m_origMsg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return KMime::Message::Ptr();
    }
    receiptTo.remove(QChar::fromLatin1('\n'));

    KMime::Message::Ptr receipt = KMime::Message::Ptr(new KMime::Message);
    const uint originalIdentity = identityUoid(m_origMsg);
    MessageHelper::initFromMessage(receipt, m_origMsg, m_identityManager, originalIdentity);
    receipt->to()->fromUnicodeString(receiptTo, QStringLiteral("utf-8").toLatin1());
    receipt->subject()->fromUnicodeString(i18n("Receipt: ") + m_origMsg->subject()->asUnicodeString(), "utf-8");

    QString str = QStringLiteral("Your message was successfully delivered.");
    str += QLatin1String("\n\n---------- Message header follows ----------\n");
    str += QString::fromLatin1(m_origMsg->head());
    str += QLatin1String("--------------------------------------------\n");
    // Conversion to toLatin1 is correct here as Mail headers should contain
    // ascii only
    receipt->setBody(str.toLatin1());
    MessageHelper::setAutomaticFields(receipt);
    receipt->assemble();

    return receipt;
}

KMime::Message::Ptr MessageFactoryNG::createMDN(KMime::MDN::ActionMode a, KMime::MDN::DispositionType d, KMime::MDN::SendingMode s, int mdnQuoteOriginal,
                                                const QVector<KMime::MDN::DispositionModifier> &m)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = m_origMsg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return KMime::Message::Ptr(new KMime::Message);
    }
    receiptTo.remove(QChar::fromLatin1('\n'));

    QString special; // fill in case of error, warning or failure

    // extract where to send from:
    QString finalRecipient = m_identityManager->identityForUoidOrDefault(identityUoid(m_origMsg)).fullEmailAddr();

    //
    // Generate message:
    //

    KMime::Message::Ptr receipt(new KMime::Message());
    const uint originalIdentity = identityUoid(m_origMsg);
    MessageHelper::initFromMessage(receipt, m_origMsg, m_identityManager, originalIdentity);
    receipt->contentType()->from7BitString("multipart/report");
    receipt->contentType()->setBoundary(KMime::multiPartBoundary());
    receipt->contentType()->setCharset("us-ascii");
    receipt->removeHeader<KMime::Headers::ContentTransferEncoding>();
    // Modify the ContentType directly (replaces setAutomaticFields(true))
    receipt->contentType()->setParameter(QStringLiteral("report-type"), QStringLiteral("disposition-notification"));

    QString description = replaceHeadersInString(m_origMsg, KMime::MDN::descriptionFor(d, m));

    // text/plain part:
    KMime::Content *firstMsgPart = new KMime::Content(m_origMsg.data());
    firstMsgPart->contentType()->setMimeType("text/plain");
    firstMsgPart->contentType()->setCharset("utf-8");
    firstMsgPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    firstMsgPart->setBody(description.toUtf8());
    receipt->addContent(firstMsgPart);

    // message/disposition-notification part:
    KMime::Content *secondMsgPart = new KMime::Content(m_origMsg.data());
    secondMsgPart->contentType()->setMimeType("message/disposition-notification");

    secondMsgPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    QByteArray originalRecipient = "";
    if (auto hrd = m_origMsg->headerByType("Original-Recipient")) {
        originalRecipient = hrd->as7BitString(false);
    }
    secondMsgPart->setBody(KMime::MDN::dispositionNotificationBodyContent(
                               finalRecipient,
                               originalRecipient,
                               m_origMsg->messageID()->as7BitString(false), /* Message-ID */
                               d, a, s, m, special));
    receipt->addContent(secondMsgPart);

    if (mdnQuoteOriginal < 0 || mdnQuoteOriginal > 2) {
        mdnQuoteOriginal = 0;
    }
    /* 0=> Nothing, 1=>Full Message, 2=>HeadersOnly*/

    KMime::Content *thirdMsgPart = new KMime::Content(m_origMsg.data());
    switch (mdnQuoteOriginal) {
    case 1:
        thirdMsgPart->contentType()->setMimeType("message/rfc822");
        thirdMsgPart->setBody(MessageCore::StringUtil::asSendableString(m_origMsg));
        receipt->addContent(thirdMsgPart);
        break;
    case 2:
        thirdMsgPart->contentType()->setMimeType("text/rfc822-headers");
        thirdMsgPart->setBody(MessageCore::StringUtil::headerAsSendableString(m_origMsg));
        receipt->addContent(thirdMsgPart);
        break;
    case 0:
    default:
        delete thirdMsgPart;
        break;
    }

    receipt->to()->fromUnicodeString(receiptTo, "utf-8");
    //Laurent: We don't translate subject ?
    receipt->subject()->from7BitString("Message Disposition Notification");
    KMime::Headers::InReplyTo *header = new KMime::Headers::InReplyTo;
    header->fromUnicodeString(m_origMsg->messageID()->asUnicodeString(), "utf-8");
    receipt->setHeader(header);

    receipt->references()->from7BitString(getRefStr(m_origMsg));

    receipt->assemble();

    qCDebug(MESSAGECOMPOSER_LOG) << "final message:" + receipt->encodedContent();

    receipt->assemble();
    return receipt;
}

QPair< KMime::Message::Ptr, KMime::Content * > MessageFactoryNG::createForwardDigestMIME(const Akonadi::Item::List &items)
{
    KMime::Message::Ptr msg(new KMime::Message);
    KMime::Content *digest = new KMime::Content(msg.data());

    QString mainPartText = i18n("\nThis is a MIME digest forward. The content of the"
                                " message is contained in the attachment(s).\n\n\n");

    digest->contentType()->setMimeType("multipart/digest");
    digest->contentType()->setBoundary(KMime::multiPartBoundary());
    digest->contentDescription()->fromUnicodeString(QStringLiteral("Digest of %1 messages.").arg(items.count()), "utf8");
    digest->contentDisposition()->setFilename(QStringLiteral("digest"));
    digest->fromUnicodeString(mainPartText);

    int id = 0;
    for (const Akonadi::Item &item : qAsConst(items)) {
        KMime::Message::Ptr fMsg = MessageComposer::Util::message(item);
        if (id == 0) {
            if (auto hrd = fMsg->headerByType("X-KMail-Identity")) {
                id = hrd->asUnicodeString().toInt();
            }
        }

        MessageCore::StringUtil::removePrivateHeaderFields(fMsg);
        fMsg->removeHeader<KMime::Headers::Bcc>();
        fMsg->assemble();
        KMime::Content *part = new KMime::Content(digest);

        part->contentType()->setMimeType("message/rfc822");
        part->contentType()->setCharset(fMsg->contentType()->charset());
        part->contentID()->setIdentifier(fMsg->contentID()->identifier());
        part->contentDescription()->fromUnicodeString(fMsg->contentDescription()->asUnicodeString(), "utf8");
        part->contentDisposition()->setParameter(QStringLiteral("name"), i18n("forwarded message"));
        part->fromUnicodeString(QString::fromLatin1(fMsg->encodedContent()));
        part->assemble();
        MessageComposer::Util::addLinkInformation(msg, item.id(), Akonadi::MessageStatus::statusForwarded());
        digest->addContent(part);
    }
    digest->assemble();

    id = m_folderId;
    MessageHelper::initHeader(msg, m_identityManager, id);

    //   qCDebug(MESSAGECOMPOSER_LOG) << "digest:" << digest->contents().size() << digest->encodedContent();

    return QPair< KMime::Message::Ptr, KMime::Content * >(msg, digest);
}

void MessageFactoryNG::setIdentityManager(KIdentityManagement::IdentityManager *ident)
{
    m_identityManager = ident;
}

void MessageFactoryNG::setReplyStrategy(MessageComposer::ReplyStrategy replyStrategy)
{
    m_replyStrategy = replyStrategy;
}

void MessageFactoryNG::setSelection(const QString &selection)
{
    m_selection = selection;
}

void MessageFactoryNG::setQuote(bool quote)
{
    m_quote = quote;
}

void MessageFactoryNG::setTemplate(const QString &templ)
{
    m_template = templ;
}

void MessageFactoryNG::setMailingListAddresses(const KMime::Types::Mailbox::List &listAddresses)
{
    m_mailingListAddresses << listAddresses;
}

void MessageFactoryNG::setFolderIdentity(Akonadi::Collection::Id folderIdentityId)
{
    m_folderId = folderIdentityId;
}

void MessageFactoryNG::putRepliesInSameFolder(Akonadi::Collection::Id parentColId)
{
    m_parentFolderId = parentColId;
}

bool MessageFactoryNG::MDNRequested(const KMime::Message::Ptr &msg)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = msg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return false;
    }
    receiptTo.remove(QChar::fromLatin1('\n'));
    return !receiptTo.isEmpty();
}

bool MessageFactoryNG::MDNConfirmMultipleRecipients(const KMime::Message::Ptr &msg)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = msg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return false;
    }
    receiptTo.remove(QChar::fromLatin1('\n'));

    // RFC 2298: [ Confirmation from the user SHOULD be obtained (or no
    // MDN sent) ] if there is more than one distinct address in the
    // Disposition-Notification-To header.
    qCDebug(MESSAGECOMPOSER_LOG) << "KEmailAddress::splitAddressList(receiptTo):"
                                 << KEmailAddress::splitAddressList(receiptTo).join(QLatin1Char('\n'));

    return KEmailAddress::splitAddressList(receiptTo).count() > 1;
}

bool MessageFactoryNG::MDNReturnPathEmpty(const KMime::Message::Ptr &msg)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = msg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return false;
    }
    receiptTo.remove(QChar::fromLatin1('\n'));

    // RFC 2298: MDNs SHOULD NOT be sent automatically if the address in
    // the Disposition-Notification-To header differs from the address
    // in the Return-Path header. [...] Confirmation from the user
    // SHOULD be obtained (or no MDN sent) if there is no Return-Path
    // header in the message [...]
    KMime::Types::AddrSpecList returnPathList = MessageHelper::extractAddrSpecs(msg, "Return-Path");
    QString returnPath = returnPathList.isEmpty() ? QString()
                         : returnPathList.front().localPart + QChar::fromLatin1('@') + returnPathList.front().domain;
    qCDebug(MESSAGECOMPOSER_LOG) << "clean return path:" << returnPath;
    return returnPath.isEmpty();
}

bool MessageFactoryNG::MDNReturnPathNotInRecieptTo(const KMime::Message::Ptr &msg)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = msg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return false;
    }
    receiptTo.remove(QChar::fromLatin1('\n'));

    // RFC 2298: MDNs SHOULD NOT be sent automatically if the address in
    // the Disposition-Notification-To header differs from the address
    // in the Return-Path header. [...] Confirmation from the user
    // SHOULD be obtained (or no MDN sent) if there is no Return-Path
    // header in the message [...]
    KMime::Types::AddrSpecList returnPathList = MessageHelper::extractAddrSpecs(msg, QStringLiteral("Return-Path").toLatin1());
    QString returnPath = returnPathList.isEmpty() ? QString()
                         : returnPathList.front().localPart + QChar::fromLatin1('@') + returnPathList.front().domain;
    qCDebug(MESSAGECOMPOSER_LOG) << "clean return path:" << returnPath;
    return !receiptTo.contains(returnPath, Qt::CaseSensitive);
}

bool MessageFactoryNG::MDNMDNUnknownOption(const KMime::Message::Ptr &msg)
{
    // RFC 2298: An importance of "required" indicates that
    // interpretation of the parameter is necessary for proper
    // generation of an MDN in response to this request.  If a UA does
    // not understand the meaning of the parameter, it MUST NOT generate
    // an MDN with any disposition type other than "failed" in response
    // to the request.
    QString notificationOptions;
    if (auto hrd = msg->headerByType("Disposition-Notification-Options")) {
        notificationOptions = hrd->asUnicodeString();
    }
    if (notificationOptions.contains(QLatin1String("required"), Qt::CaseSensitive)) {
        // ### hacky; should parse...
        // There is a required option that we don't understand. We need to
        // ask the user what we should do:
        return true;
    }
    return false;
}

uint MessageFactoryNG::identityUoid(const KMime::Message::Ptr &msg)
{
    QString idString;
    if (auto hdr = msg->headerByType("X-KMail-Identity")) {
        idString = hdr->asUnicodeString().trimmed();
    }
    bool ok = false;
    uint id = idString.toUInt(&ok);

    if (!ok || id == 0) {
        id = m_identityManager->identityForAddress(msg->to()->asUnicodeString() + QLatin1String(", ") + msg->cc()->asUnicodeString()).uoid();
    }

    if (id == 0 && m_folderId > 0) {
        id = m_folderId;
    }
    return id;
}

QString MessageFactoryNG::replaceHeadersInString(const KMime::Message::Ptr &msg, const QString &s)
{
    QString result = s;
    QRegExp rx(QStringLiteral("\\$\\{([a-z0-9-]+)\\}"), Qt::CaseInsensitive);
    Q_ASSERT(rx.isValid());

    QRegExp rxDate(QStringLiteral("\\$\\{date\\}"));
    Q_ASSERT(rxDate.isValid());

    const QString sDate = KMime::DateFormatter::formatDate(
        KMime::DateFormatter::Localized, msg->date()->dateTime().toTime_t());
    qCDebug(MESSAGECOMPOSER_LOG) << "creating mdn date:" << msg->date()->dateTime().toTime_t() << sDate;

    int idx = 0;
    if ((idx = rxDate.indexIn(result, idx)) != -1) {
        result.replace(idx, rxDate.matchedLength(), sDate);
    }

    idx = 0;
    while ((idx = rx.indexIn(result, idx)) != -1) {
        const QByteArray ba = rx.cap(1).toLatin1();
        QString replacement;
        if (auto hrd = msg->headerByType(ba.constData())) {
            replacement = hrd->asUnicodeString();
        }
        result.replace(idx, rx.matchedLength(), replacement);
        idx += replacement.length();
    }
    return result;
}

void MessageFactoryNG::applyCharset(const KMime::Message::Ptr msg)
{
    if (MessageComposer::MessageComposerSettings::forceReplyCharset()) {
        // first convert the body from its current encoding to unicode representation
        QTextCodec *bodyCodec = KCharsets::charsets()->codecForName(QString::fromLatin1(msg->contentType()->charset()));
        if (!bodyCodec) {
            bodyCodec = KCharsets::charsets()->codecForName(QStringLiteral("UTF-8"));
        }

        const QString body = bodyCodec->toUnicode(msg->body());

        // then apply the encoding of the original message
        msg->contentType()->setCharset(m_origMsg->contentType()->charset());

        QTextCodec *codec = KCharsets::charsets()->codecForName(QString::fromLatin1(msg->contentType()->charset()));
        if (!codec) {
            qCCritical(MESSAGECOMPOSER_LOG) << "Could not get text codec for charset" << msg->contentType()->charset();
        } else if (!codec->canEncode(body)) {     // charset can't encode body, fall back to preferred
            const QStringList charsets = MessageComposer::MessageComposerSettings::preferredCharsets();

            QList<QByteArray> chars;
            chars.reserve(charsets.count());
            for (const QString &charset : charsets) {
                chars << charset.toLatin1();
            }

            QByteArray fallbackCharset = MessageComposer::Util::selectCharset(chars, body);
            if (fallbackCharset.isEmpty()) { // UTF-8 as fall-through
                fallbackCharset = "UTF-8";
            }

            codec = KCharsets::charsets()->codecForName(QString::fromLatin1(fallbackCharset));
            msg->setBody(codec->fromUnicode(body));
        } else {
            msg->setBody(codec->fromUnicode(body));
        }
    }
}

QByteArray MessageFactoryNG::getRefStr(const KMime::Message::Ptr &msg)
{
    QByteArray firstRef, lastRef, refStr, retRefStr;
    int i, j;

    if (auto hdr = msg->references(false)) {
        refStr = hdr->as7BitString(false).trimmed();
    }

    if (refStr.isEmpty()) {
        return msg->messageID()->as7BitString(false);
    }

    i = refStr.indexOf('<');
    j = refStr.indexOf('>');
    firstRef = refStr.mid(i, j - i + 1);
    if (!firstRef.isEmpty()) {
        retRefStr = firstRef + ' ';
    }

    i = refStr.lastIndexOf('<');
    j = refStr.lastIndexOf('>');

    lastRef = refStr.mid(i, j - i + 1);
    if (!lastRef.isEmpty() && lastRef != firstRef) {
        retRefStr += lastRef + ' ';
    }

    retRefStr += msg->messageID()->as7BitString(false);
    return retRefStr;
}
