/*
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "messagefactoryng.h"

#include "draftstatus/draftstatus.h"
#include "messagefactoryforwardjob.h"
#include "messagefactoryreplyjob.h"
#include "settings/messagecomposersettings.h"
#include <MessageComposer/Util>

#include <KCursorSaver>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>

#include "helper/messagehelper.h"
#include "messagecomposer_debug.h"
#include <KEmailAddress>
#include <KLocalizedString>
#include <MessageCore/DateFormatter>
#include <MessageCore/MailingList>
#include <MessageCore/StringUtil>
#include <MessageCore/Util>
#include <QRegularExpression>
#include <QStringDecoder>
#include <QStringEncoder>

using namespace MessageComposer;

namespace KMime
{
namespace Types
{
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
static KMime::Types::Mailbox::List stripMyAddressesFromAddressList(const KMime::Types::Mailbox::List &list,
                                                                   const KIdentityManagementCore::IdentityManager *manager)
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
    , mOrigMsg(origMsg)
    , mFolderId(0)
    , mParentFolderId(0)
    , mCollection(col)
    , mReplyStrategy(MessageComposer::ReplySmart)
    , mId(id)
{
}

MessageFactoryNG::~MessageFactoryNG() = default;

// Return the addresses to use when replying to the author of msg.
// See <https://cr.yp.to/proto/replyto.html>.
static KMime::Types::Mailbox::List authorMailboxes(const KMime::Message::Ptr &msg, const KMime::Types::Mailbox::List &mailingLists)
{
    if (auto mrt = msg->headerByType("Mail-Reply-To")) {
        return KMime::Types::Mailbox::listFrom7BitString(mrt->as7BitString(false));
    }
    if (auto rt = msg->replyTo(false)) {
        // Did a mailing list munge Reply-To?
        auto mboxes = rt->mailboxes();
        for (const auto &list : mailingLists) {
            mboxes.removeAll(list);
        }
        if (!mboxes.isEmpty()) {
            return mboxes;
        }
    }
    return msg->from(true)->mailboxes();
}

void MessageFactoryNG::slotCreateReplyDone(const KMime::Message::Ptr &msg, bool replyAll)
{
    MessageComposer::Util::addLinkInformation(msg, mId, Akonadi::MessageStatus::statusReplied());
    if (mParentFolderId > 0) {
        auto header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(QString::number(mParentFolderId));
        msg->setHeader(header);
    }

    if (DraftEncryptionState(mOrigMsg).encryptionState()) {
        DraftEncryptionState(msg).setState(true);
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

    const uint originalIdentity = identityUoid(mOrigMsg);
    MessageHelper::initFromMessage(msg, mOrigMsg, mIdentityManager, originalIdentity);
    if (auto replyTo = mOrigMsg->replyTo(false)) {
        replyToList = replyTo->mailboxes();
    }

    msg->contentType()->setCharset("utf-8");

    if (auto hdr = mOrigMsg->headerByType("List-Post")) {
        static const QRegularExpression rx{QStringLiteral("<\\s*mailto\\s*:([^@>]+@[^>]+)>"), QRegularExpression::CaseInsensitiveOption};
        const auto match = rx.match(hdr->asUnicodeString());
        if (match.hasMatch()) {
            KMime::Types::Mailbox mailbox;
            mailbox.fromUnicodeString(match.captured(1));
            mMailingListAddresses << mailbox;
        }
    }

    switch (mReplyStrategy) {
    case MessageComposer::ReplySmart: {
        if (auto hdr = mOrigMsg->headerByType("Mail-Followup-To")) {
            toList << KMime::Types::Mailbox::listFrom7BitString(hdr->as7BitString(false));
        } else if (!mMailingListAddresses.isEmpty()) {
            if (replyToList.isEmpty()) {
                toList = (KMime::Types::Mailbox::List() << mMailingListAddresses.at(0));
            } else {
                toList = replyToList;
            }
        } else {
            // Doesn't seem to be a mailing list.
            auto originalFromList = mOrigMsg->from()->mailboxes();
            auto originalToList = mOrigMsg->to()->mailboxes();

            if (mIdentityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(originalFromList))
                && !mIdentityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(originalToList))) {
                // Sender seems to be one of our own identities and recipient is not,
                // so we assume that this is a reply to a "sent" mail where the user
                // wants to add additional information for the recipient.
                toList = originalToList;
            } else {
                // "Normal" case:  reply to sender.
                toList = authorMailboxes(mOrigMsg, mMailingListAddresses);
            }

            replyAll = false;
        }
        // strip all my addresses from the list of recipients
        const KMime::Types::Mailbox::List recipients = toList;

        toList = stripMyAddressesFromAddressList(recipients, mIdentityManager);

        // ... unless the list contains only my addresses (reply to self)
        if (toList.isEmpty() && !recipients.isEmpty()) {
            toList << recipients.first();
        }
        break;
    }
    case MessageComposer::ReplyList: {
        if (auto hdr = mOrigMsg->headerByType("Mail-Followup-To")) {
            KMime::Types::Mailbox mailbox;
            mailbox.from7BitString(hdr->as7BitString(false));
            toList << mailbox;
        } else if (!mMailingListAddresses.isEmpty()) {
            toList << mMailingListAddresses[0];
        } else if (!replyToList.isEmpty()) {
            // assume a Reply-To header mangling mailing list
            toList = replyToList;
        }

        // strip all my addresses from the list of recipients
        const KMime::Types::Mailbox::List recipients = toList;

        toList = stripMyAddressesFromAddressList(recipients, mIdentityManager);
        break;
    }
    case MessageComposer::ReplyAll:
        if (auto hdr = mOrigMsg->headerByType("Mail-Followup-To")) {
            toList = KMime::Types::Mailbox::listFrom7BitString(hdr->as7BitString(false));
        } else {
            auto ccList = stripMyAddressesFromAddressList(mOrigMsg->cc()->mailboxes(), mIdentityManager);

            if (!mMailingListAddresses.isEmpty()) {
                toList = stripMyAddressesFromAddressList(mOrigMsg->to()->mailboxes(), mIdentityManager);
                bool addMailingList = true;
                for (const KMime::Types::Mailbox &m : std::as_const(mMailingListAddresses)) {
                    if (toList.contains(m)) {
                        addMailingList = false;
                        break;
                    }
                }
                if (addMailingList) {
                    toList += mMailingListAddresses.front();
                }

                ccList += authorMailboxes(mOrigMsg, mMailingListAddresses);
            } else {
                // Doesn't seem to be a mailing list.
                auto originalFromList = mOrigMsg->from()->mailboxes();
                auto originalToList = mOrigMsg->to()->mailboxes();
                if (mIdentityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(originalFromList))
                    && !mIdentityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(originalToList))) {
                    // Sender seems to be one of our own identities and recipient is not,
                    // so we assume that this is a reply to a "sent" mail where the user
                    // wants to add additional information for the recipient.
                    toList = originalToList;
                } else {
                    // "Normal" case:  reply to sender.
                    toList = stripMyAddressesFromAddressList(mOrigMsg->to()->mailboxes(), mIdentityManager);
                    const auto authors = authorMailboxes(mOrigMsg, mMailingListAddresses);
                    if (toList.isEmpty() || !mIdentityManager->thatIsMe(KMime::Types::Mailbox::listToUnicodeString(authors))) {
                        toList += authors;
                    }
                }
            }

            for (const KMime::Types::Mailbox &mailbox : std::as_const(ccList)) {
                msg->cc()->addAddress(mailbox);
            }
        }
        break;
    case MessageComposer::ReplyAuthor:
        toList = authorMailboxes(mOrigMsg, mMailingListAddresses);
        replyAll = false;
        break;
    case MessageComposer::ReplyNone:
        // the addressees will be set by the caller
        break;
    default:
        Q_UNREACHABLE();
    }

    for (const KMime::Types::Mailbox &mailbox : std::as_const(toList)) {
        msg->to()->addAddress(mailbox);
    }

    refStr = getRefStr(mOrigMsg);
    if (!refStr.isEmpty()) {
        msg->references()->fromUnicodeString(QString::fromLocal8Bit(refStr));
    }
    // In-Reply-To = original msg-id
    msg->inReplyTo()->from7BitString(mOrigMsg->messageID()->as7BitString(false));

    msg->subject()->fromUnicodeString(MessageCore::StringUtil::replySubject(mOrigMsg.data()));

    // If the reply shouldn't be blank, apply the template to the message
    if (mQuote) {
        auto job = new MessageFactoryReplyJob;
        connect(job, &MessageFactoryReplyJob::replyDone, this, &MessageFactoryNG::slotCreateReplyDone);
        job->setMsg(msg);
        job->setReplyAll(replyAll);
        job->setIdentityManager(mIdentityManager);
        job->setSelection(mSelection);
        job->setTemplate(mTemplate);
        job->setOrigMsg(mOrigMsg);
        job->setCollection(mCollection);
        job->setReplyAsHtml(mReplyAsHtml);
        job->start();
    } else {
        slotCreateReplyDone(msg, replyAll);
    }
}

void MessageFactoryNG::slotCreateForwardDone(const KMime::Message::Ptr &msg)
{
    MessageComposer::Util::addLinkInformation(msg, mId, Akonadi::MessageStatus::statusForwarded());
    msg->assemble();
    Q_EMIT createForwardDone(msg);
}

void MessageFactoryNG::createForwardAsync()
{
    KMime::Message::Ptr msg(new KMime::Message);

    // This is a non-multipart, non-text mail (e.g. text/calendar). Construct
    // a multipart/mixed mail and add the original body as an attachment.
    if (!mOrigMsg->contentType()->isMultipart()
        && (!mOrigMsg->contentType(false)->isText()
            || (mOrigMsg->contentType(false)->isText() && mOrigMsg->contentType(false)->subType() != "html"
                && mOrigMsg->contentType(false)->subType() != "plain"))) {
        const uint originalIdentity = identityUoid(mOrigMsg);
        MessageHelper::initFromMessage(msg, mOrigMsg, mIdentityManager, originalIdentity);
        msg->removeHeader<KMime::Headers::ContentType>();
        msg->removeHeader<KMime::Headers::ContentTransferEncoding>();

        msg->contentType(true)->setMimeType("multipart/mixed");

        // TODO: Andras: somebody should check if this is correct. :)
        // empty text part
        auto msgPart = new KMime::Content;
        msgPart->contentType()->setMimeType("text/plain");
        msg->appendContent(msgPart);

        // the old contents of the mail
        auto secondPart = new KMime::Content;
        secondPart->contentType()->setMimeType(mOrigMsg->contentType()->mimeType());
        secondPart->setBody(mOrigMsg->body());
        // use the headers of the original mail
        secondPart->setHead(mOrigMsg->head());
        msg->appendContent(secondPart);
        msg->assemble();
    }
    // Normal message (multipart or text/plain|html)
    // Just copy the message, the template parser will do the hard work of
    // replacing the body text in TemplateParser::addProcessedBodyToMessage()
    else {
        // TODO Check if this is ok
        msg->setHead(mOrigMsg->head());
        msg->setBody(mOrigMsg->body());
        const QString oldContentType = msg->contentType(true)->asUnicodeString();
        const uint originalIdentity = identityUoid(mOrigMsg);
        MessageHelper::initFromMessage(msg, mOrigMsg, mIdentityManager, originalIdentity);

        // restore the content type, MessageHelper::initFromMessage() sets the contents type to
        // text/plain, via initHeader(), for unclear reasons
        msg->contentType()->fromUnicodeString(oldContentType);
        msg->assemble();
    }

    msg->subject()->fromUnicodeString(MessageCore::StringUtil::forwardSubject(mOrigMsg.data()));
    auto job = new MessageFactoryForwardJob;
    connect(job, &MessageFactoryForwardJob::forwardDone, this, &MessageFactoryNG::slotCreateForwardDone);
    job->setIdentityManager(mIdentityManager);
    job->setMsg(msg);
    job->setSelection(mSelection);
    job->setTemplate(mTemplate);
    job->setOrigMsg(mOrigMsg);
    job->setCollection(mCollection);
    job->start();
}

QPair<KMime::Message::Ptr, QList<KMime::Content *>> MessageFactoryNG::createAttachedForward(const Akonadi::Item::List &items)
{
    // create forwarded message with original message as attachment
    // remove headers that shouldn't be forwarded
    KMime::Message::Ptr msg(new KMime::Message);
    QList<KMime::Content *> attachments;

    const int numberOfItems(items.count());
    if (numberOfItems >= 2) {
        // don't respect X-KMail-Identity headers because they might differ for
        // the selected mails
        MessageHelper::initHeader(msg, mIdentityManager, 0);
    } else if (numberOfItems == 1) {
        KMime::Message::Ptr firstMsg = MessageComposer::Util::message(items.first());
        const uint originalIdentity = identityUoid(firstMsg);
        MessageHelper::initFromMessage(msg, firstMsg, mIdentityManager, originalIdentity);
        msg->subject()->fromUnicodeString(MessageCore::StringUtil::forwardSubject(firstMsg.data()));
    }

    MessageHelper::setAutomaticFields(msg, true);
    KCursorSaver saver(Qt::WaitCursor);
    if (numberOfItems == 0) {
        attachments << createForwardAttachmentMessage(mOrigMsg);
        MessageComposer::Util::addLinkInformation(msg, mId, Akonadi::MessageStatus::statusForwarded());
    } else {
        // iterate through all the messages to be forwarded
        attachments.reserve(items.count());
        for (const Akonadi::Item &item : std::as_const(items)) {
            attachments << createForwardAttachmentMessage(MessageComposer::Util::message(item));
            MessageComposer::Util::addLinkInformation(msg, item.id(), Akonadi::MessageStatus::statusForwarded());
        }
    }

    // msg->assemble();
    return QPair<KMime::Message::Ptr, QList<KMime::Content *>>(msg, QList<KMime::Content *>() << attachments);
}

KMime::Content *MessageFactoryNG::createForwardAttachmentMessage(const KMime::Message::Ptr &fwdMsg)
{
    // remove headers that shouldn't be forwarded
    MessageCore::StringUtil::removePrivateHeaderFields(fwdMsg);
    fwdMsg->removeHeader<KMime::Headers::Bcc>();
    fwdMsg->assemble();
    // set the part
    auto msgPart = new KMime::Content(fwdMsg.data());
    auto ct = msgPart->contentType();
    ct->setMimeType("message/rfc822");

    auto cd = msgPart->contentDisposition(); // create
    cd->setParameter(QByteArrayLiteral("filename"), i18n("forwarded message"));
    cd->setDisposition(KMime::Headers::CDinline);
    const QString subject = fwdMsg->subject()->asUnicodeString();
    ct->setParameter(QByteArrayLiteral("name"), subject);
    cd->fromUnicodeString(fwdMsg->from()->asUnicodeString() + QLatin1StringView(": ") + subject);
    msgPart->setBody(fwdMsg->encodedContent());
    msgPart->assemble();

    MessageComposer::Util::addLinkInformation(fwdMsg, 0, Akonadi::MessageStatus::statusForwarded());
    return msgPart;
}

bool MessageFactoryNG::replyAsHtml() const
{
    return mReplyAsHtml;
}

void MessageFactoryNG::setReplyAsHtml(bool replyAsHtml)
{
    mReplyAsHtml = replyAsHtml;
}

KMime::Message::Ptr MessageFactoryNG::createResend()
{
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(mOrigMsg->encodedContent());
    msg->parse();
    msg->removeHeader<KMime::Headers::MessageID>();
    uint originalIdentity = identityUoid(mOrigMsg);

    // Set the identity from above
    auto header = new KMime::Headers::Generic("X-KMail-Identity");
    header->fromUnicodeString(QString::number(originalIdentity));
    msg->setHeader(header);

    // Restore the original bcc field as this is overwritten in applyIdentity
    msg->bcc(mOrigMsg->bcc());
    return msg;
}

KMime::Message::Ptr
MessageFactoryNG::createRedirect(const QString &toStr, const QString &ccStr, const QString &bccStr, int transportId, const QString &fcc, int identity)
{
    if (!mOrigMsg) {
        return {};
    }

    // copy the message 1:1
    KMime::Message::Ptr msg(new KMime::Message);
    msg->setContent(mOrigMsg->encodedContent());
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
    const KIdentityManagementCore::Identity &ident = mIdentityManager->identityForUoidOrDefault(id);

    // X-KMail-Redirect-From: content
    const QString strByWayOf =
        QString::fromLocal8Bit("%1 (by way of %2 <%3>)").arg(mOrigMsg->from()->asUnicodeString(), ident.fullName(), ident.primaryEmailAddress());

    // Resent-From: content
    const QString strFrom = QString::fromLocal8Bit("%1 <%2>").arg(ident.fullName(), ident.primaryEmailAddress());

    // format the current date to be used in Resent-Date:
    // FIXME: generate datetime the same way as KMime, otherwise we get inconsistency
    // in unit-tests. Unfortunately RFC2822Date is not enough for us, we need the
    // composition hack below
    const QDateTime dt = QDateTime::currentDateTime();
    const QString newDate = QLocale::c().toString(dt, QStringLiteral("ddd, ")) + dt.toString(Qt::RFC2822Date);

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
    auto header = new KMime::Headers::Generic("Resent-Message-ID");
    header->fromUnicodeString(MessageCore::StringUtil::generateMessageId(msg->sender()->asUnicodeString(), msgIdSuffix));
    msg->setHeader(header);

    header = new KMime::Headers::Generic("Resent-Date");
    header->fromUnicodeString(newDate);
    msg->setHeader(header);

    header = new KMime::Headers::Generic("Resent-From");
    header->fromUnicodeString(strFrom);
    msg->setHeader(header);

    if (msg->to(false)) {
        auto headerT = new KMime::Headers::To;
        headerT->fromUnicodeString(mOrigMsg->to()->asUnicodeString());
        msg->setHeader(headerT);
    }

    header = new KMime::Headers::Generic("Resent-To");
    header->fromUnicodeString(toStr);
    msg->setHeader(header);

    if (!ccStr.isEmpty()) {
        header = new KMime::Headers::Generic("Resent-Cc");
        header->fromUnicodeString(ccStr);
        msg->setHeader(header);
    }

    if (!bccStr.isEmpty()) {
        header = new KMime::Headers::Generic("Resent-Bcc");
        header->fromUnicodeString(bccStr);
        msg->setHeader(header);
    }

    header = new KMime::Headers::Generic("X-KMail-Redirect-From");
    header->fromUnicodeString(strByWayOf);
    msg->setHeader(header);

    if (transportId != -1) {
        header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(QString::number(transportId));
        msg->setHeader(header);
    }

    if (!fcc.isEmpty()) {
        header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(fcc);
        msg->setHeader(header);
    }

    const bool fccIsDisabled = ident.disabledFcc();
    if (fccIsDisabled) {
        header = new KMime::Headers::Generic("X-KMail-FccDisabled");
        header->fromUnicodeString(QStringLiteral("true"));
        msg->setHeader(header);
    } else {
        msg->removeHeader("X-KMail-FccDisabled");
    }

    msg->assemble();

    MessageComposer::Util::addLinkInformation(msg, mId, Akonadi::MessageStatus::statusForwarded());
    return msg;
}

KMime::Message::Ptr MessageFactoryNG::createDeliveryReceipt()
{
    QString receiptTo;
    if (auto hrd = mOrigMsg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return {};
    }
    receiptTo.remove(QLatin1Char('\n'));

    KMime::Message::Ptr receipt(new KMime::Message);
    const uint originalIdentity = identityUoid(mOrigMsg);
    MessageHelper::initFromMessage(receipt, mOrigMsg, mIdentityManager, originalIdentity);
    receipt->to()->fromUnicodeString(receiptTo);
    receipt->subject()->fromUnicodeString(i18n("Receipt: ") + mOrigMsg->subject()->asUnicodeString());

    QString str = QStringLiteral("Your message was successfully delivered.");
    str += QLatin1StringView("\n\n---------- Message header follows ----------\n");
    str += QString::fromLatin1(mOrigMsg->head());
    str += QLatin1StringView("--------------------------------------------\n");
    // Conversion to toLatin1 is correct here as Mail headers should contain
    // ascii only
    receipt->setBody(str.toLatin1());
    MessageHelper::setAutomaticFields(receipt);
    receipt->assemble();

    return receipt;
}

KMime::Message::Ptr MessageFactoryNG::createMDN(KMime::MDN::ActionMode a,
                                                KMime::MDN::DispositionType d,
                                                KMime::MDN::SendingMode s,
                                                int mdnQuoteOriginal,
                                                const QList<KMime::MDN::DispositionModifier> &m)
{
    // extract where to send to:
    QString receiptTo;
    if (auto hrd = mOrigMsg->headerByType("Disposition-Notification-To")) {
        receiptTo = hrd->asUnicodeString();
    }
    if (receiptTo.trimmed().isEmpty()) {
        return KMime::Message::Ptr(new KMime::Message);
    }
    receiptTo.remove(QLatin1Char('\n'));

    QString special; // fill in case of error, warning or failure

    // extract where to send from:
    QString finalRecipient = mIdentityManager->identityForUoidOrDefault(identityUoid(mOrigMsg)).fullEmailAddr();

    //
    // Generate message:
    //

    KMime::Message::Ptr receipt(new KMime::Message());
    const uint originalIdentity = identityUoid(mOrigMsg);
    MessageHelper::initFromMessage(receipt, mOrigMsg, mIdentityManager, originalIdentity);
    auto contentType = receipt->contentType(true); // create it
    contentType->from7BitString("multipart/report");
    contentType->setBoundary(KMime::multiPartBoundary());
    contentType->setCharset("us-ascii");
    receipt->removeHeader<KMime::Headers::ContentTransferEncoding>();
    // Modify the ContentType directly (replaces setAutomaticFields(true))
    contentType->setParameter(QByteArrayLiteral("report-type"), QStringLiteral("disposition-notification"));

    const QString description = replaceHeadersInString(mOrigMsg, KMime::MDN::descriptionFor(d, m));

    // text/plain part:
    auto firstMsgPart = new KMime::Content(mOrigMsg.data());
    auto firstMsgPartContentType = firstMsgPart->contentType(); // create it
    firstMsgPartContentType->setMimeType("text/plain");
    firstMsgPartContentType->setCharset("utf-8");
    firstMsgPart->contentTransferEncoding(true)->setEncoding(KMime::Headers::CE7Bit);
    firstMsgPart->setBody(description.toUtf8());
    receipt->appendContent(firstMsgPart);

    // message/disposition-notification part:
    auto secondMsgPart = new KMime::Content(mOrigMsg.data());
    secondMsgPart->contentType()->setMimeType("message/disposition-notification");

    secondMsgPart->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
    QByteArray originalRecipient = "";
    if (auto hrd = mOrigMsg->headerByType("Original-Recipient")) {
        originalRecipient = hrd->as7BitString(false);
    }
    secondMsgPart->setBody(KMime::MDN::dispositionNotificationBodyContent(finalRecipient,
                                                                          originalRecipient,
                                                                          mOrigMsg->messageID()->as7BitString(false), /* Message-ID */
                                                                          d,
                                                                          a,
                                                                          s,
                                                                          m,
                                                                          special));
    receipt->appendContent(secondMsgPart);

    if ((mdnQuoteOriginal < 0) || (mdnQuoteOriginal > 2)) {
        mdnQuoteOriginal = 0;
    }
    /* 0=> Nothing, 1=>Full Message, 2=>HeadersOnly*/

    auto thirdMsgPart = new KMime::Content(mOrigMsg.data());
    switch (mdnQuoteOriginal) {
    case 1:
        thirdMsgPart->contentType()->setMimeType("message/rfc822");
        thirdMsgPart->setBody(MessageCore::StringUtil::asSendableString(mOrigMsg));
        receipt->appendContent(thirdMsgPart);
        break;
    case 2:
        thirdMsgPart->contentType()->setMimeType("text/rfc822-headers");
        thirdMsgPart->setBody(MessageCore::StringUtil::headerAsSendableString(mOrigMsg));
        receipt->appendContent(thirdMsgPart);
        break;
    case 0:
    default:
        delete thirdMsgPart;
        break;
    }

    receipt->to()->fromUnicodeString(receiptTo);
    // Laurent: We don't translate subject ?
    receipt->subject()->from7BitString("Message Disposition Notification");
    auto header = new KMime::Headers::InReplyTo;
    header->fromUnicodeString(mOrigMsg->messageID()->asUnicodeString());
    receipt->setHeader(header);

    receipt->references()->from7BitString(getRefStr(mOrigMsg));

    receipt->assemble();

    qCDebug(MESSAGECOMPOSER_LOG) << "final message:" + receipt->encodedContent();

    receipt->assemble();
    return receipt;
}

QPair<KMime::Message::Ptr, KMime::Content *> MessageFactoryNG::createForwardDigestMIME(const Akonadi::Item::List &items)
{
    KMime::Message::Ptr msg(new KMime::Message);
    auto digest = new KMime::Content(msg.data());

    const QString mainPartText = i18n(
        "\nThis is a MIME digest forward. The content of the"
        " message is contained in the attachment(s).\n\n\n");

    auto ct = digest->contentType();
    ct->setMimeType("multipart/digest");
    ct->setBoundary(KMime::multiPartBoundary());
    digest->contentDescription()->fromUnicodeString(QStringLiteral("Digest of %1 messages.").arg(items.count()));
    digest->contentDisposition()->setFilename(QStringLiteral("digest"));
    digest->fromUnicodeString(mainPartText);

    int id = 0;
    for (const Akonadi::Item &item : std::as_const(items)) {
        KMime::Message::Ptr fMsg = MessageComposer::Util::message(item);
        if (id == 0) {
            if (auto hrd = fMsg->headerByType("X-KMail-Identity")) {
                id = hrd->asUnicodeString().toInt();
            }
        }

        MessageCore::StringUtil::removePrivateHeaderFields(fMsg);
        fMsg->removeHeader<KMime::Headers::Bcc>();
        fMsg->assemble();
        auto part = new KMime::Content(digest);

        part->contentType()->setMimeType("message/rfc822");
        part->contentType(false)->setCharset(fMsg->contentType()->charset());
        part->contentID()->setIdentifier(fMsg->contentID()->identifier());
        part->contentDescription()->fromUnicodeString(fMsg->contentDescription()->asUnicodeString());
        part->contentDisposition()->setParameter(QByteArrayLiteral("name"), i18n("forwarded message"));
        part->fromUnicodeString(QString::fromLatin1(fMsg->encodedContent()));
        part->assemble();
        MessageComposer::Util::addLinkInformation(msg, item.id(), Akonadi::MessageStatus::statusForwarded());
        digest->appendContent(part);
    }
    digest->assemble();

    id = mFolderId;
    MessageHelper::initHeader(msg, mIdentityManager, id);

    //   qCDebug(MESSAGECOMPOSER_LOG) << "digest:" << digest->contents().size() << digest->encodedContent();

    return QPair<KMime::Message::Ptr, KMime::Content *>(msg, digest);
}

void MessageFactoryNG::setIdentityManager(KIdentityManagementCore::IdentityManager *ident)
{
    mIdentityManager = ident;
}

void MessageFactoryNG::setReplyStrategy(MessageComposer::ReplyStrategy replyStrategy)
{
    mReplyStrategy = replyStrategy;
}

void MessageFactoryNG::setSelection(const QString &selection)
{
    mSelection = selection;
}

void MessageFactoryNG::setQuote(bool quote)
{
    mQuote = quote;
}

void MessageFactoryNG::setTemplate(const QString &templ)
{
    mTemplate = templ;
}

void MessageFactoryNG::setMailingListAddresses(const KMime::Types::Mailbox::List &listAddresses)
{
    mMailingListAddresses << listAddresses;
}

void MessageFactoryNG::setFolderIdentity(uint folderIdentityId)
{
    mFolderId = folderIdentityId;
}

void MessageFactoryNG::putRepliesInSameFolder(Akonadi::Collection::Id parentColId)
{
    mParentFolderId = parentColId;
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
    receiptTo.remove(QLatin1Char('\n'));
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
    receiptTo.remove(QLatin1Char('\n'));

    // RFC 2298: [ Confirmation from the user SHOULD be obtained (or no
    // MDN sent) ] if there is more than one distinct address in the
    // Disposition-Notification-To header.
    qCDebug(MESSAGECOMPOSER_LOG) << "KEmailAddress::splitAddressList(receiptTo):" << KEmailAddress::splitAddressList(receiptTo).join(QLatin1Char('\n'));

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
    receiptTo.remove(QLatin1Char('\n'));

    // RFC 2298: MDNs SHOULD NOT be sent automatically if the address in
    // the Disposition-Notification-To header differs from the address
    // in the Return-Path header. [...] Confirmation from the user
    // SHOULD be obtained (or no MDN sent) if there is no Return-Path
    // header in the message [...]
    KMime::Types::AddrSpecList returnPathList = MessageHelper::extractAddrSpecs(msg, "Return-Path");
    const QString returnPath = returnPathList.isEmpty() ? QString() : returnPathList.front().localPart + QLatin1Char('@') + returnPathList.front().domain;
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
    receiptTo.remove(QLatin1Char('\n'));

    // RFC 2298: MDNs SHOULD NOT be sent automatically if the address in
    // the Disposition-Notification-To header differs from the address
    // in the Return-Path header. [...] Confirmation from the user
    // SHOULD be obtained (or no MDN sent) if there is no Return-Path
    // header in the message [...]
    KMime::Types::AddrSpecList returnPathList = MessageHelper::extractAddrSpecs(msg, QStringLiteral("Return-Path").toLatin1());
    const QString returnPath = returnPathList.isEmpty() ? QString() : returnPathList.front().localPart + QLatin1Char('@') + returnPathList.front().domain;
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
    if (notificationOptions.contains(QLatin1StringView("required"), Qt::CaseSensitive)) {
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
        id = MessageCore::Util::identityForMessage(msg.data(), mIdentityManager, mFolderId).uoid();
    }
    return id;
}

QString MessageFactoryNG::replaceHeadersInString(const KMime::Message::Ptr &msg, const QString &s)
{
    QString result = s;
    static QRegularExpression rx{QStringLiteral("\\$\\{([a-z0-9-]+)\\}"), QRegularExpression::CaseInsensitiveOption};

    const QString sDate = MessageCore::DateFormatter::formatDate(MessageCore::DateFormatter::Localized, msg->date()->dateTime());
    qCDebug(MESSAGECOMPOSER_LOG) << "creating mdn date:" << msg->date()->dateTime().toSecsSinceEpoch() << sDate;

    result.replace(QStringLiteral("${date}"), sDate);

    int idx = 0;
    for (auto match = rx.match(result); match.hasMatch(); match = rx.match(result, idx)) {
        idx = match.capturedStart(0);
        const QByteArray ba = match.captured(1).toLatin1();
        if (auto hdr = msg->headerByType(ba.constData())) {
            const auto replacement = hdr->asUnicodeString();
            result.replace(idx, match.capturedLength(0), replacement);
            idx += replacement.length();
        } else {
            result.remove(idx, match.capturedLength(0));
        }
    }
    return result;
}

QByteArray MessageFactoryNG::getRefStr(const KMime::Message::Ptr &msg)
{
    QByteArray firstRef;
    QByteArray lastRef;
    QByteArray refStr;
    QByteArray retRefStr;
    int i;
    int j;

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

#include "moc_messagefactoryng.cpp"
