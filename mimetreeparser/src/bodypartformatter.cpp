/*  -*- c++ -*-
    bodypartformatter.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "bodyformatter/applicationpgpencrypted.h"

#include "bodyformatter/applicationpkcs7mime.h"
#include "bodyformatter/encrypted.h"
#include "bodyformatter/mailman.h"
#include "bodyformatter/multipartalternative.h"
#include "bodyformatter/multipartencrypted.h"
#include "bodyformatter/multipartmixed.h"
#include "bodyformatter/multipartsigned.h"
#include "bodyformatter/texthtml.h"
#include "bodyformatter/textplain.h"

#include "interfaces/bodypart.h"
#include "interfaces/bodypartformatter.h"

#include "bodypartformatterfactory_p.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

using namespace MimeTreeParser;
using namespace Qt::Literals::StringLiterals;

namespace
{
class AnyTypeBodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
public:
    MessagePart::Ptr process(Interface::BodyPart &part) const override
    {
        KMime::Content *node = part.content();
        const auto mp = AttachmentMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, part.source()->decryptMessage()));
        part.processResult()->setInlineSignatureState(mp->signatureState());
        part.processResult()->setInlineEncryptionState(mp->encryptionState());
        part.processResult()->setNeverDisplayInline(true);
        mp->setNeverDisplayInline(true);
        mp->setIsImage(false);
        return mp;
    }
};

class ImageTypeBodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
public:
    MessagePart::Ptr process(Interface::BodyPart &part) const override
    {
        KMime::Content *node = part.content();
        auto mp = AttachmentMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, part.source()->decryptMessage()));
        mp->setIsImage(true);
        part.processResult()->setInlineSignatureState(mp->signatureState());
        part.processResult()->setInlineEncryptionState(mp->encryptionState());

        auto preferredMode = part.source()->preferredMode();
        const bool isHtmlPreferred = (preferredMode == Util::Html) || (preferredMode == Util::MultipartHtml);
        const auto parentContentType = node->parent() ? node->parent()->contentType(KMime::CreatePolicy::DontCreate) : nullptr;
        if (parentContentType && parentContentType->subType() == "related" && isHtmlPreferred) {
            part.nodeHelper()->setNodeDisplayedEmbedded(node, true);
            part.nodeHelper()->setNodeDisplayedHidden(node, true);
            return mp;
        }

        return mp;
    }
};

class MessageRfc822BodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
public:
    MessagePart::Ptr process(Interface::BodyPart &) const override;
};

MessagePart::Ptr MessageRfc822BodyPartFormatter::process(Interface::BodyPart &part) const
{
    const std::shared_ptr<KMime::Message> message = part.content()->bodyAsMessage();
    return MessagePart::Ptr(new EncapsulatedRfc822MessagePart(part.objectTreeParser(), part.content(), message));
}
} // anon namespace

void BodyPartFormatterFactoryPrivate::messageviewer_create_builtin_bodypart_formatters()
{
    insert(u"application/pkcs7-mime"_s, std::make_unique<ApplicationPkcs7MimeBodyPartFormatter>());
    insert(u"application/x-pkcs7-mime"_s, std::make_unique<ApplicationPkcs7MimeBodyPartFormatter>());
    insert(u"application/pgp-encrypted"_s, std::make_unique<ApplicationPGPEncryptedBodyPartFormatter>());

    insert(u"application/octet-stream"_s, std::make_unique<ApplicationPkcs7MimeBodyPartFormatter>());
    insert(u"application/octet-stream"_s, std::make_unique<EncryptedBodyPartFormatter>(EncryptedBodyPartFormatter::AutoPGP));
    insert(u"application/octet-stream"_s, std::make_unique<AnyTypeBodyPartFormatter>());

    insert(u"text/pgp"_s, std::make_unique<EncryptedBodyPartFormatter>(EncryptedBodyPartFormatter::ForcePGP));
    insert(u"text/html"_s, std::make_unique<TextHtmlBodyPartFormatter>());
    insert(u"text/rtf"_s, std::make_unique<AnyTypeBodyPartFormatter>());
    insert(u"text/plain"_s, std::make_unique<MailmanBodyPartFormatter>());
    insert(u"text/plain"_s, std::make_unique<TextPlainBodyPartFormatter>());

    insert(u"image/png"_s, std::make_unique<ImageTypeBodyPartFormatter>());
    insert(u"image/jpeg"_s, std::make_unique<ImageTypeBodyPartFormatter>());
    insert(u"image/gif"_s, std::make_unique<ImageTypeBodyPartFormatter>());
    insert(u"image/svg+xml"_s, std::make_unique<ImageTypeBodyPartFormatter>());
    insert(u"image/bmp"_s, std::make_unique<ImageTypeBodyPartFormatter>());
    insert(u"image/vnd.microsoft.icon"_s, std::make_unique<ImageTypeBodyPartFormatter>());

    insert(u"message/rfc822"_s, std::make_unique<MessageRfc822BodyPartFormatter>());

    insert(u"multipart/alternative"_s, std::make_unique<MultiPartAlternativeBodyPartFormatter>());
    insert(u"multipart/encrypted"_s, std::make_unique<MultiPartEncryptedBodyPartFormatter>());
    insert(u"multipart/signed"_s, std::make_unique<MultiPartSignedBodyPartFormatter>());
    insert(u"multipart/mixed"_s, std::make_unique<MultiPartMixedBodyPartFormatter>());
}
