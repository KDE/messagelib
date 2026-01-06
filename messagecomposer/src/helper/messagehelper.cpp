/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "messagehelper.h"
using namespace Qt::Literals::StringLiterals;

#include "MessageCore/MailingList"
#include "MessageCore/StringUtil"

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <KMime/MDN>

using namespace MessageCore;

namespace MessageHelper
{
void initHeader(const std::shared_ptr<KMime::Message> &message, const KIdentityManagementCore::IdentityManager *identMan, uint id)
{
    applyIdentity(message, identMan, id);
    message->removeHeader<KMime::Headers::To>();
    message->removeHeader<KMime::Headers::Subject>();
    message->date()->setDateTime(QDateTime::currentDateTime());

    // This will allow to change Content-Type:
    message->contentType()->setMimeType("text/plain");
}

void initFromMessage(const std::shared_ptr<KMime::Message> &msg,
                     const std::shared_ptr<KMime::Message> &origMsg,
                     KIdentityManagementCore::IdentityManager *identMan,
                     uint id,
                     bool idHeaders)
{
    if (idHeaders) {
        MessageHelper::initHeader(msg, identMan, id);
    } else {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Identity");
        header->fromUnicodeString(QString::number(id));
        msg->setHeader(std::move(header));
    }

    if (auto hdr = origMsg->headerByType("X-KMail-Transport")) {
        const QString transport = hdr->asUnicodeString();
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Transport");
        header->fromUnicodeString(transport);
        msg->setHeader(std::move(header));
    }
}

void applyIdentity(const std::shared_ptr<KMime::Message> &message, const KIdentityManagementCore::IdentityManager *identMan, uint id)
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
            message->bcc(KMime::CreatePolicy::Create)->addAddress(mailbox);
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
        auto organization = std::make_unique<KMime::Headers::Organization>();
        organization->fromUnicodeString(ident.organization());
        message->setHeader(std::move(organization));
    }

    if (ident.isDefault()) {
        message->removeHeader("X-KMail-Identity");
    } else {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Identity");
        header->fromUnicodeString(QString::number(ident.uoid()));
        message->setHeader(std::move(header));
    }

    if (ident.transport().isEmpty()) {
        message->removeHeader("X-KMail-Transport");
    } else {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Transport");
        header->fromUnicodeString(ident.transport());
        message->setHeader(std::move(header));
    }

    if (ident.fcc().isEmpty()) {
        message->removeHeader("X-KMail-Fcc");
    } else {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-Fcc");
        header->fromUnicodeString(ident.fcc());
        message->setHeader(std::move(header));
    }

    if (ident.disabledFcc()) {
        auto header = std::make_unique<KMime::Headers::Generic>("X-KMail-FccDisabled");
        header->fromUnicodeString(u"true"_s);
        message->setHeader(std::move(header));
    } else {
        message->removeHeader("X-KMail-FccDisabled");
    }
}

QList<KMime::Types::AddrSpec> extractAddrSpecs(const std::shared_ptr<const KMime::Message> &msg, const QByteArray &header)
{
    QList<KMime::Types::AddrSpec> result;
    if (auto hrd = msg->headerByType(header.constData())) {
        // Don't use "asUnicodeString().toUtf8()" it removes \" from \"foo, bla\" <foo@kde.org> => send failed
        // Bug 439218
        const auto al = MessageCore::StringUtil::splitAddressField(hrd->as7BitString());
        for (const auto &a : al) {
            for (const auto &m : a.mailboxList) {
                result.push_back(m.addrSpec());
            }
        }
    }
    return result;
}

void setAutomaticFields(const std::shared_ptr<KMime::Message> &msg, bool aIsMulti)
{
    auto header = msg->header<KMime::Headers::MIMEVersion>(KMime::CreatePolicy::Create);
    header->from7BitString("1.0");

    if (aIsMulti || msg->contents().size() > 1) {
        // Set the type to 'Multipart' and the subtype to 'Mixed'
        msg->contentType(KMime::CreatePolicy::DontCreate)->setMimeType("multipart/mixed");
        // Create a random printable string and set it as the boundary parameter
        msg->contentType(KMime::CreatePolicy::DontCreate)->setBoundary(KMime::multiPartBoundary());
    }
}
}
