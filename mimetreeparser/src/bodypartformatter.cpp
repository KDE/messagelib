/*  -*- c++ -*-
    bodypartformatter.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "bodyformatter/applicationpgpencrypted.h"
using namespace Qt::Literals::StringLiterals;

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
    insert(u"application/pkcs7-mime"_s, ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(u"application/x-pkcs7-mime"_s, ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(u"application/pgp-encrypted"_s, ApplicationPGPEncryptedBodyPartFormatter::create());

    insert(u"application/octet-stream"_s, ApplicationPkcs7MimeBodyPartFormatter::create());
    insert(u"application/octet-stream"_s, EncryptedBodyPartFormatter::create(EncryptedBodyPartFormatter::AutoPGP));
    insert(u"application/octet-stream"_s, AnyTypeBodyPartFormatter::create());

    insert(u"text/pgp"_s, EncryptedBodyPartFormatter::create(EncryptedBodyPartFormatter::ForcePGP));
    insert(u"text/html"_s, TextHtmlBodyPartFormatter::create());
    insert(u"text/rtf"_s, AnyTypeBodyPartFormatter::create());
    insert(u"text/plain"_s, MailmanBodyPartFormatter::create());
    insert(u"text/plain"_s, TextPlainBodyPartFormatter::create());

    insert(u"image/png"_s, ImageTypeBodyPartFormatter::create());
    insert(u"image/jpeg"_s, ImageTypeBodyPartFormatter::create());
    insert(u"image/gif"_s, ImageTypeBodyPartFormatter::create());
    insert(u"image/svg+xml"_s, ImageTypeBodyPartFormatter::create());
    insert(u"image/bmp"_s, ImageTypeBodyPartFormatter::create());
    insert(u"image/vnd.microsoft.icon"_s, ImageTypeBodyPartFormatter::create());

    insert(u"message/rfc822"_s, MessageRfc822BodyPartFormatter::create());

    insert(u"multipart/alternative"_s, MultiPartAlternativeBodyPartFormatter::create());
    insert(u"multipart/encrypted"_s, MultiPartEncryptedBodyPartFormatter::create());
    insert(u"multipart/signed"_s, MultiPartSignedBodyPartFormatter::create());
    insert(u"multipart/mixed"_s, MultiPartMixedBodyPartFormatter::create());
}
