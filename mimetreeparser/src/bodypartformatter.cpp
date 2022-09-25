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

namespace
{
class AnyTypeBodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
    static const AnyTypeBodyPartFormatter *self;

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

    static const MimeTreeParser::Interface::BodyPartFormatter *create()
    {
        if (!self) {
            self = new AnyTypeBodyPartFormatter();
        }
        return self;
    }
};

const AnyTypeBodyPartFormatter *AnyTypeBodyPartFormatter::self = nullptr;

class ImageTypeBodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
    static const ImageTypeBodyPartFormatter *self;

public:
    static const MimeTreeParser::Interface::BodyPartFormatter *create()
    {
        if (!self) {
            self = new ImageTypeBodyPartFormatter();
        }
        return self;
    }

    MessagePart::Ptr process(Interface::BodyPart &part) const override
    {
        KMime::Content *node = part.content();
        auto mp = AttachmentMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, part.source()->decryptMessage()));
        mp->setIsImage(true);
        part.processResult()->setInlineSignatureState(mp->signatureState());
        part.processResult()->setInlineEncryptionState(mp->encryptionState());

        auto preferredMode = part.source()->preferredMode();
        const bool isHtmlPreferred = (preferredMode == Util::Html) || (preferredMode == Util::MultipartHtml);
        if (node->parent() && node->parent()->contentType()->subType() == "related" && isHtmlPreferred) {
            part.nodeHelper()->setNodeDisplayedEmbedded(node, true);
            part.nodeHelper()->setNodeDisplayedHidden(node, true);
            return mp;
        }

        return mp;
    }
};

const ImageTypeBodyPartFormatter *ImageTypeBodyPartFormatter::self = nullptr;

class MessageRfc822BodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
    static const MessageRfc822BodyPartFormatter *self;

public:
    MessagePart::Ptr process(Interface::BodyPart &) const override;
    static const MimeTreeParser::Interface::BodyPartFormatter *create();
};

const MessageRfc822BodyPartFormatter *MessageRfc822BodyPartFormatter::self;

const MimeTreeParser::Interface::BodyPartFormatter *MessageRfc822BodyPartFormatter::create()
{
    if (!self) {
        self = new MessageRfc822BodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr MessageRfc822BodyPartFormatter::process(Interface::BodyPart &part) const
{
    const KMime::Message::Ptr message = part.content()->bodyAsMessage();
    return MessagePart::Ptr(new EncapsulatedRfc822MessagePart(part.objectTreeParser(), part.content(), message));
}
} // anon namespace

void BodyPartFormatterFactoryPrivate::messageviewer_create_builtin_bodypart_formatters()
{
    insert(QStringLiteral("application/pkcs7-mime"), ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(QStringLiteral("application/x-pkcs7-mime"), ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(QStringLiteral("application/pgp-encrypted"), ApplicationPGPEncryptedBodyPartFormatter::create());

    insert(QStringLiteral("application/octet-stream"), ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(QStringLiteral("application/octet-stream"), EncryptedBodyPartFormatter::create(EncryptedBodyPartFormatter::AutoPGP));
    insert(QStringLiteral("application/octet-stream"), AnyTypeBodyPartFormatter::create());

    insert(QStringLiteral("text/pgp"), EncryptedBodyPartFormatter::create(EncryptedBodyPartFormatter::ForcePGP));
    insert(QStringLiteral("text/html"), TextHtmlBodyPartFormatter::create());
    insert(QStringLiteral("text/rtf"), AnyTypeBodyPartFormatter::create());
    insert(QStringLiteral("text/plain"), MailmanBodyPartFormatter::create());
    insert(QStringLiteral("text/plain"), TextPlainBodyPartFormatter::create());

    insert(QStringLiteral("image/png"), ImageTypeBodyPartFormatter::create());
    insert(QStringLiteral("image/jpeg"), ImageTypeBodyPartFormatter::create());
    insert(QStringLiteral("image/gif"), ImageTypeBodyPartFormatter::create());
    insert(QStringLiteral("image/svg+xml"), ImageTypeBodyPartFormatter::create());
    insert(QStringLiteral("image/bmp"), ImageTypeBodyPartFormatter::create());
    insert(QStringLiteral("image/vnd.microsoft.icon"), ImageTypeBodyPartFormatter::create());

    insert(QStringLiteral("message/rfc822"), MessageRfc822BodyPartFormatter::create());

    insert(QStringLiteral("multipart/alternative"), MultiPartAlternativeBodyPartFormatter::create());
    insert(QStringLiteral("multipart/encrypted"), MultiPartEncryptedBodyPartFormatter::create());
    insert(QStringLiteral("multipart/signed"), MultiPartSignedBodyPartFormatter::create());
    insert(QStringLiteral("multipart/mixed"), MultiPartMixedBodyPartFormatter::create());
}
