/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "messagehelper.h"

#include "MessageCore/MailingList"
#include "MessageCore/StringUtil"

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KMime/MDN>
#include <KMime/Message>

using namespace MessageCore;

namespace MessageHelper
{
void initHeader(const KMime::Message::Ptr &message, const KIdentityManagementCore::IdentityManager *identMan, uint id)
{
    applyIdentity(message, identMan, id);
    message->removeHeader<KMime::Headers::To>();
    message->removeHeader<KMime::Headers::Subject>();
    message->date()->setDateTime(QDateTime::currentDateTime());

    // This will allow to change Content-Type:
    message->contentType()->setMimeType("text/plain");
}

void initFromMessage(const KMime::Message::Ptr &msg,
                     const KMime::Message::Ptr &origMsg,
                     KIdentityManagementCore::IdentityManager *identMan,
                     uint id,
                     bool idHeaders)
{
    if (idHeaders) {
        MessageHelper::initHeader(msg, identMan, id);
    } else {
        auto header = new KMime::Headers::Generic("X-KMail-Identity");
        header->fromUnicodeString(QString::number(id));
        msg->setHeader(header);
    }

    if (auto hdr = origMsg->headerByType("X-KMail-Transport")) {
        const QString transport = hdr->asUnicodeString();
        auto header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(transport);
        msg->setHeader(header);
    }
}

void applyIdentity(const KMime::Message::Ptr &message, const KIdentityManagementCore::IdentityManager *identMan, uint id)
{
    const KIdentityManagementCore::Identity &ident = identMan->identityForUoidOrDefault(id);

    if (ident.fullEmailAddr().isEmpty()) {
        message->removeHeader<KMime::Headers::From>();
    } else {
        message->from()->addAddress(ident.primaryEmailAddress().toUtf8(), ident.fullName());
    }

    if (ident.replyToAddr().isEmpty()) {
        message->removeHeader<KMime::Headers::ReplyTo>();
    } else {
        message->replyTo()->addAddress(ident.replyToAddr().toUtf8());
    }

    if (ident.bcc().isEmpty()) {
        message->removeHeader<KMime::Headers::Bcc>();
    } else {
        const auto mailboxes = KMime::Types::Mailbox::listFromUnicodeString(ident.bcc());
        for (const KMime::Types::Mailbox &mailbox : mailboxes) {
            message->bcc()->addAddress(mailbox);
        }
    }

    if (ident.cc().isEmpty()) {
        message->removeHeader<KMime::Headers::Cc>();
    } else {
        const auto mailboxes = KMime::Types::Mailbox::listFromUnicodeString(ident.cc());
        for (const KMime::Types::Mailbox &mailbox : mailboxes) {
            message->cc()->addAddress(mailbox);
        }
    }

    if (ident.organization().isEmpty()) {
        message->removeHeader<KMime::Headers::Organization>();
    } else {
        auto const organization = new KMime::Headers::Organization;
        organization->fromUnicodeString(ident.organization());
        message->setHeader(organization);
    }

    if (ident.isDefault()) {
        message->removeHeader("X-KMail-Identity");
    } else {
        auto header = new KMime::Headers::Generic("X-KMail-Identity");
        header->fromUnicodeString(QString::number(ident.uoid()));
        message->setHeader(header);
    }

    if (ident.transport().isEmpty()) {
        message->removeHeader("X-KMail-Transport");
    } else {
        auto header = new KMime::Headers::Generic("X-KMail-Transport");
        header->fromUnicodeString(ident.transport());
        message->setHeader(header);
    }

    if (ident.fcc().isEmpty()) {
        message->removeHeader("X-KMail-Fcc");
    } else {
        auto header = new KMime::Headers::Generic("X-KMail-Fcc");
        header->fromUnicodeString(ident.fcc());
        message->setHeader(header);
    }

    if (ident.disabledFcc()) {
        auto header = new KMime::Headers::Generic("X-KMail-FccDisabled");
        header->fromUnicodeString(QStringLiteral("true"));
        message->setHeader(header);
    } else {
        message->removeHeader("X-KMail-FccDisabled");
    }
}

KMime::Types::AddrSpecList extractAddrSpecs(const KMime::Message::Ptr &msg, const QByteArray &header)
{
    KMime::Types::AddrSpecList result;
    if (auto hrd = msg->headerByType(header.constData())) {
        // Don't use "asUnicodeString().toUtf8()" it removes \" from \"foo, bla\" <foo@kde.org> => send failed
        // Bug 439218
        KMime::Types::AddressList al = MessageCore::StringUtil::splitAddressField(hrd->as7BitString(false));
        KMime::Types::AddressList::const_iterator alend(al.constEnd());
        for (KMime::Types::AddressList::const_iterator ait = al.constBegin(); ait != alend; ++ait) {
            KMime::Types::MailboxList::const_iterator mitEnd((*ait).mailboxList.constEnd());
            for (KMime::Types::MailboxList::const_iterator mit = (*ait).mailboxList.constBegin(); mit != mitEnd; ++mit) {
                result.push_back((*mit).addrSpec());
            }
        }
    }
    return result;
}

void setAutomaticFields(const KMime::Message::Ptr &msg, bool aIsMulti)
{
    auto header = msg->header<KMime::Headers::MIMEVersion>(true);
    header->from7BitString("1.0");

    if (aIsMulti || msg->contents().size() > 1) {
        // Set the type to 'Multipart' and the subtype to 'Mixed'
        msg->contentType(false)->setMimeType("multipart/mixed");
        // Create a random printable string and set it as the boundary parameter
        msg->contentType(false)->setBoundary(KMime::multiPartBoundary());
    }
}
}
