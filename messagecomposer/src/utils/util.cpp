/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>
  Copyright (C) 2009 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  Copyright (c) 2009 Leo Franchi <lfranchi@kde.org>

  Parts based on KMail code by:

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "utils/util.h"
#include "util_p.h"

#include <QRegularExpression>
#include "composer/composer.h"
#include "job/singlepartjob.h"

#include <QTextCodec>
#include <QTextDocument>
#include <QTextBlock>

#include <KCharsets>
#include "messagecomposer_debug.h"
#include <KLocalizedString>
#include <KMessageBox>

#include <kmime/kmime_content.h>
#include <kmime/kmime_headers.h>
#include <mailtransportakonadi/messagequeuejob.h>
#include <AkonadiCore/agentinstance.h>
#include <AkonadiCore/agentinstancecreatejob.h>
#include <AkonadiCore/agentmanager.h>
#include <MessageCore/StringUtil>

KMime::Content *setBodyAndCTE(QByteArray &encodedBody, KMime::Headers::ContentType *contentType, KMime::Content *ret)
{
    MessageComposer::Composer composer;
    MessageComposer::SinglepartJob cteJob(&composer);

    cteJob.contentType()->setMimeType(contentType->mimeType());
    cteJob.contentType()->setCharset(contentType->charset());
    cteJob.setData(encodedBody);
    cteJob.exec();
    cteJob.content()->assemble();

    ret->contentTransferEncoding()->setEncoding(cteJob.contentTransferEncoding()->encoding());
    ret->setBody(cteJob.content()->encodedBody());

    return ret;
}

KMime::Content *MessageComposer::Util::composeHeadersAndBody(KMime::Content *orig, QByteArray encodedBody, Kleo::CryptoMessageFormat format, bool sign, const QByteArray &hashAlgo)
{
    KMime::Content *result = new KMime::Content;

    // called should have tested that the signing/encryption failed
    Q_ASSERT(!encodedBody.isEmpty());

    if (!(format & Kleo::InlineOpenPGPFormat)) {    // make a MIME message
        qCDebug(MESSAGECOMPOSER_LOG) << "making MIME message, format:" << format;
        makeToplevelContentType(result, format, sign, hashAlgo);

        if (makeMultiMime(format, sign)) {      // sign/enc PGPMime, sign SMIME
            const QByteArray boundary = KMime::multiPartBoundary();
            result->contentType()->setBoundary(boundary);

            result->assemble();
            //qCDebug(MESSAGECOMPOSER_LOG) << "processed header:" << result->head();

            // Build the encapsulated MIME parts.
            // Build a MIME part holding the code information
            // taking the body contents returned in ciphertext.
            KMime::Content *code = new KMime::Content;
            setNestedContentType(code, format, sign);
            setNestedContentDisposition(code, format, sign);

            if (sign) {                           // sign PGPMime, sign SMIME
                if (format & Kleo::AnySMIME) {      // sign SMIME
                    code->contentTransferEncoding()->setEncoding(KMime::Headers::CEbase64);
                    code->contentTransferEncoding()->needToEncode();
                    code->setBody(encodedBody);
                } else {                            // sign PGPMmime
                    setBodyAndCTE(encodedBody, orig->contentType(), code);
                }
                result->addContent(orig);
                result->addContent(code);
            } else {                              // enc PGPMime
                setBodyAndCTE(encodedBody, orig->contentType(), code);

                // Build a MIME part holding the version information
                // taking the body contents returned in
                // structuring.data.bodyTextVersion.
                KMime::Content *vers = new KMime::Content;
                vers->contentType()->setMimeType("application/pgp-encrypted");
                vers->contentDisposition()->setDisposition(KMime::Headers::CDattachment);
                vers->contentTransferEncoding()->setEncoding(KMime::Headers::CE7Bit);
                vers->setBody("Version: 1");

                result->addContent(vers);
                result->addContent(code);
            }
        } else {                                //enc SMIME, sign/enc SMIMEOpaque
            result->contentTransferEncoding()->setEncoding(KMime::Headers::CEbase64);
            result->contentDisposition()->setDisposition(KMime::Headers::CDattachment);
            result->contentDisposition()->setFilename(QStringLiteral("smime.p7m"));

            result->assemble();
            //qCDebug(MESSAGECOMPOSER_LOG) << "processed header:" << result->head();

            result->setBody(encodedBody);
        }
    } else {                                  // sign/enc PGPInline
        result->setHead(orig->head());
        result->parse();

        // fixing ContentTransferEncoding
        setBodyAndCTE(encodedBody, orig->contentType(), result);
    }
    return result;
}

// set the correct top-level ContentType on the message
void MessageComposer::Util::makeToplevelContentType(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign, const QByteArray &hashAlgo)
{
    switch (format) {
    default:
    case Kleo::InlineOpenPGPFormat:
    case Kleo::OpenPGPMIMEFormat:
        if (sign) {
            content->contentType()->setMimeType(QByteArrayLiteral("multipart/signed"));
            content->contentType()->setParameter(QStringLiteral("protocol"), QStringLiteral("application/pgp-signature"));
            content->contentType()->setParameter(QStringLiteral("micalg"), QString::fromLatin1(QByteArray(QByteArrayLiteral("pgp-") + hashAlgo)).toLower());
        } else {
            content->contentType()->setMimeType(QByteArrayLiteral("multipart/encrypted"));
            content->contentType()->setParameter(QStringLiteral("protocol"), QStringLiteral("application/pgp-encrypted"));
        }
        return;
    case Kleo::SMIMEFormat:
        if (sign) {
            qCDebug(MESSAGECOMPOSER_LOG) << "setting headers for SMIME";
            content->contentType()->setMimeType(QByteArrayLiteral("multipart/signed"));
            content->contentType()->setParameter(QStringLiteral("protocol"), QStringLiteral("application/pkcs7-signature"));
            content->contentType()->setParameter(QStringLiteral("micalg"), QString::fromLatin1(hashAlgo).toLower());
            return;
        }
        // fall through (for encryption, there's no difference between
        // SMIME and SMIMEOpaque, since there is no mp/encrypted for
        // S/MIME)
        Q_FALLTHROUGH();
    case Kleo::SMIMEOpaqueFormat:

        qCDebug(MESSAGECOMPOSER_LOG) << "setting headers for SMIME/opaque";
        content->contentType()->setMimeType(QByteArrayLiteral("application/pkcs7-mime"));

        if (sign) {
            content->contentType()->setParameter(QStringLiteral("smime-type"), QStringLiteral("signed-data"));
        } else {
            content->contentType()->setParameter(QStringLiteral("smime-type"), QStringLiteral("enveloped-data"));
        }
        content->contentType()->setParameter(QStringLiteral("name"), QStringLiteral("smime.p7m"));
    }
}

void MessageComposer::Util::setNestedContentType(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign)
{
    switch (format) {
    case Kleo::OpenPGPMIMEFormat:
        if (sign) {
            content->contentType()->setMimeType(QByteArrayLiteral("application/pgp-signature"));
            content->contentType()->setParameter(QStringLiteral("name"), QStringLiteral("signature.asc"));
            content->contentDescription()->from7BitString("This is a digitally signed message part.");
        } else {
            content->contentType()->setMimeType(QByteArrayLiteral("application/octet-stream"));
        }
        return;
    case Kleo::SMIMEFormat:
        if (sign) {
            content->contentType()->setMimeType(QByteArrayLiteral("application/pkcs7-signature"));
            content->contentType()->setParameter(QStringLiteral("name"), QStringLiteral("smime.p7s"));
            return;
        }
        Q_FALLTHROUGH();
    // fall through:
    default:
    case Kleo::InlineOpenPGPFormat:
    case Kleo::SMIMEOpaqueFormat:
        ;
    }
}

void MessageComposer::Util::setNestedContentDisposition(KMime::Content *content, Kleo::CryptoMessageFormat format, bool sign)
{
    if (!sign && format & Kleo::OpenPGPMIMEFormat) {
        content->contentDisposition()->setDisposition(KMime::Headers::CDinline);
        content->contentDisposition()->setFilename(QStringLiteral("msg.asc"));
    } else if (sign && format & Kleo::SMIMEFormat) {
        content->contentDisposition()->setDisposition(KMime::Headers::CDattachment);
        content->contentDisposition()->setFilename(QStringLiteral("smime.p7s"));
    }
}

bool MessageComposer::Util::makeMultiMime(Kleo::CryptoMessageFormat format, bool sign)
{
    switch (format) {
    default:
    case Kleo::InlineOpenPGPFormat:
    case Kleo::SMIMEOpaqueFormat:
        return false;
    case Kleo::OpenPGPMIMEFormat:
        return true;
    case Kleo::SMIMEFormat:
        return sign;                             // only on sign - there's no mp/encrypted for S/MIME
    }
}

QByteArray MessageComposer::Util::selectCharset(const QList<QByteArray> &charsets, const QString &text)
{
    for (const QByteArray &name : charsets) {
        // We use KCharsets::codecForName() instead of QTextCodec::codecForName() here, because
        // the former knows us-ascii is latin1.
        QTextCodec *codec = KCharsets::charsets()->codecForName(QString::fromLatin1(name));
        if (!codec) {
            qCWarning(MESSAGECOMPOSER_LOG) << "Could not get text codec for charset" << name;
            continue;
        }
        if (codec->canEncode(text)) {
            // Special check for us-ascii (needed because us-ascii is not exactly latin1).
            if (name == "us-ascii" && !KMime::isUsAscii(text)) {
                continue;
            }
            qCDebug(MESSAGECOMPOSER_LOG) << "Chosen charset" << name;
            return name;
        }
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "No appropriate charset found.";
    return QByteArray();
}

QStringList MessageComposer::Util::AttachmentKeywords()
{
    return i18nc(
        "comma-separated list of keywords that are used to detect whether "
        "the user forgot to attach his attachment. Do not add space between words.",
        "attachment,attached").split(QLatin1Char(','));
}

QString MessageComposer::Util::cleanedUpHeaderString(const QString &s)
{
    // remove invalid characters from the header strings
    QString res(s);
    res.remove(QChar::fromLatin1('\r'));
    res.replace(QChar::fromLatin1('\n'), QLatin1Char(' '));
    return res.trimmed();
}

void MessageComposer::Util::addSendReplyForwardAction(const KMime::Message::Ptr &message, MailTransport::MessageQueueJob *qjob)
{
    QList<Akonadi::Item::Id> originalMessageId;
    QList<Akonadi::MessageStatus> linkStatus;
    if (MessageComposer::Util::getLinkInformation(message, originalMessageId, linkStatus)) {
        for (Akonadi::Item::Id id : qAsConst(originalMessageId)) {
            if (linkStatus.first() == Akonadi::MessageStatus::statusReplied()) {
                qjob->sentActionAttribute().addAction(MailTransport::SentActionAttribute::Action::MarkAsReplied, QVariant(id));
            } else if (linkStatus.first() == Akonadi::MessageStatus::statusForwarded()) {
                qjob->sentActionAttribute().addAction(MailTransport::SentActionAttribute::Action::MarkAsForwarded, QVariant(id));
            }
        }
    }
}

bool MessageComposer::Util::sendMailDispatcherIsOnline(QWidget *parent)
{
    Akonadi::AgentInstance instance = Akonadi::AgentManager::self()->instance(QStringLiteral("akonadi_maildispatcher_agent"));
    if (!instance.isValid()) {
        const int rc = KMessageBox::warningYesNo(parent, i18n("The mail dispatcher is not set up, so mails cannot be sent. Do you want to create a mail dispatcher?"),
                                                 i18n("No mail dispatcher."), KStandardGuiItem::yes(), KStandardGuiItem::no(), QStringLiteral("no_maildispatcher"));
        if (rc == KMessageBox::Yes) {
            const Akonadi::AgentType type = Akonadi::AgentManager::self()->type(QStringLiteral("akonadi_maildispatcher_agent"));
            Q_ASSERT(type.isValid());
            Akonadi::AgentInstanceCreateJob *job = new Akonadi::AgentInstanceCreateJob(type); // async. We'll have to try again later.
            job->start();
        }
        return false;
    }
    if (instance.isOnline()) {
        return true;
    } else {
        const int rc = KMessageBox::warningYesNo(parent, i18n("The mail dispatcher is offline, so mails cannot be sent. Do you want to make it online?"),
                                                 i18n("Mail dispatcher offline."), KStandardGuiItem::yes(), KStandardGuiItem::no(), QStringLiteral("maildispatcher_put_online"));
        if (rc == KMessageBox::Yes) {
            instance.setIsOnline(true);
            return true;
        }
    }
    return false;
}

void MessageComposer::Util::removeNotNecessaryHeaders(const KMime::Message::Ptr &msg)
{
    msg->removeHeader("X-KMail-SignatureActionEnabled");
    msg->removeHeader("X-KMail-EncryptActionEnabled");
    msg->removeHeader("X-KMail-CryptoMessageFormat");
}

KMime::Content *MessageComposer::Util::findTypeInMessage(KMime::Content *data, const QByteArray &mimeType, const QByteArray &subType)
{
    if (!data->contentType()->isEmpty()) {
        if (mimeType.isEmpty() || subType.isEmpty()) {
            return data;
        }
        if ((mimeType == data->contentType()->mediaType())
            && (subType == data->contentType()->subType())) {
            return data;
        }
    }

    foreach (auto child, data->contents()) {
        if ((!child->contentType()->isEmpty())
            && (mimeType == child->contentType()->mimeType())
            && (subType == child->contentType()->subType())) {
            return child;
        }
        auto ret = findTypeInMessage(child, mimeType, subType);
        if (ret) {
            return ret;
        }
    }
    return nullptr;
}

void MessageComposer::Util::addLinkInformation(const KMime::Message::Ptr &msg, Akonadi::Item::Id id, Akonadi::MessageStatus status)
{
    Q_ASSERT(status.isReplied() || status.isForwarded() || status.isDeleted());

    QString message;
    if (auto hrd = msg->headerByType("X-KMail-Link-Message")) {
        message = hrd->asUnicodeString();
    }
    if (!message.isEmpty()) {
        message += QChar::fromLatin1(',');
    }

    QString type;
    if (auto hrd = msg->headerByType("X-KMail-Link-Type")) {
        type = hrd->asUnicodeString();
    }
    if (!type.isEmpty()) {
        type += QChar::fromLatin1(',');
    }

    message += QString::number(id);
    if (status.isReplied()) {
        type += QLatin1String("reply");
    } else if (status.isForwarded()) {
        type += QLatin1String("forward");
    }

    KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Link-Message");
    header->fromUnicodeString(message, "utf-8");
    msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Link-Type");
    header->fromUnicodeString(type, "utf-8");
    msg->setHeader(header);
}

bool MessageComposer::Util::getLinkInformation(const KMime::Message::Ptr &msg, QList<Akonadi::Item::Id> &id, QList<Akonadi::MessageStatus> &status)
{
    auto hrdLinkMsg = msg->headerByType("X-KMail-Link-Message");
    auto hrdLinkType = msg->headerByType("X-KMail-Link-Type");
    if (!hrdLinkMsg || !hrdLinkType) {
        return false;
    }

    const QStringList messages = hrdLinkMsg->asUnicodeString().split(QLatin1Char(','), QString::SkipEmptyParts);
    const QStringList types = hrdLinkType->asUnicodeString().split(QLatin1Char(','), QString::SkipEmptyParts);

    if (messages.isEmpty() || types.isEmpty()) {
        return false;
    }

    for (const QString &idStr : messages) {
        id << idStr.toLongLong();
    }

    for (const QString &typeStr : types) {
        if (typeStr == QLatin1String("reply")) {
            status << Akonadi::MessageStatus::statusReplied();
        } else if (typeStr == QLatin1String("forward")) {
            status << Akonadi::MessageStatus::statusForwarded();
        }
    }
    return true;
}

bool MessageComposer::Util::isStandaloneMessage(const Akonadi::Item &item)
{
    // standalone message have a valid payload, but are not, themselves valid items
    return item.hasPayload<KMime::Message::Ptr>() && !item.isValid();
}

KMime::Message::Ptr MessageComposer::Util::message(const Akonadi::Item &item)
{
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Payload is not a MessagePtr!";
        return KMime::Message::Ptr();
    }

    return item.payload<KMime::Message::Ptr>();
}

bool MessageComposer::Util::hasMissingAttachments(const QStringList &attachmentKeywords, QTextDocument *doc, const QString &subj)
{
    if (!doc) {
        return false;
    }
    QStringList attachWordsList = attachmentKeywords;

    QRegularExpression rx(QLatin1String("\\b")
                          +attachWordsList.join(QStringLiteral("\\b|\\b"))
                          +QLatin1String("\\b"), QRegularExpression::CaseInsensitiveOption);

    // check whether the subject contains one of the attachment key words
    // unless the message is a reply or a forwarded message
    bool gotMatch = (MessageCore::StringUtil::stripOffPrefixes(subj) == subj) && (rx.match(subj).hasMatch());

    if (!gotMatch) {
        // check whether the non-quoted text contains one of the attachment key
        // words
        QRegularExpression quotationRx(QStringLiteral("^([ \\t]*([|>:}#]|[A-Za-z]+>))+"));
        QTextBlock end(doc->end());
        for (QTextBlock it = doc->begin(); it != end; it = it.next()) {
            const QString line = it.text();
            gotMatch = (!quotationRx.match(line).hasMatch())
                       && (rx.match(line).hasMatch());
            if (gotMatch) {
                break;
            }
        }
    }

    if (!gotMatch) {
        return false;
    }
    return true;
}
