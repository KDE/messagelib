/*  -*- c++ -*-
    bodypartformatter.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003 Marc Mutz <mutz@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License, version 2, as
    published by the Free Software Foundation.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "messageviewer_debug.h"
#include "viewer/bodypartformatterfactory_p.h"
#include "viewer/attachmentstrategy.h"
#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"

#include "viewer/objecttreeparser.h"
#include "messagepart.h"

#include <kmime/kmime_content.h>

using namespace MessageViewer;

Interface::HtmlWriter* Interface::MessagePart::htmlWriter()
{
    if (!mHtmlWriter) {
        mHtmlWriter = mPart->objectTreeParser()->htmlWriter();
    }
    return mHtmlWriter;
}

void Interface::MessagePart::html(bool decorate)
{
    static_cast<QueueHtmlWriter *>(mHtmlWriter)->replay();
}


namespace
{
class AnyTypeBodyPartFormatter
    : public MessageViewer::Interface::BodyPartFormatter
{
    static const AnyTypeBodyPartFormatter *self;
public:
    Result format(Interface::BodyPart *, Interface::HtmlWriter *) const Q_DECL_OVERRIDE
    {
        qCDebug(MESSAGEVIEWER_LOG) << "Acting as a Interface::BodyPartFormatter!";
        return AsIcon;
    }

    // unhide the overload with three arguments
    using MessageViewer::Interface::BodyPartFormatter::format;

    bool process(ObjectTreeParser *, KMime::Content *, ProcessResult &result) const
    {
        result.setNeverDisplayInline(true);
        return false;
    }
    static const MessageViewer::Interface::BodyPartFormatter *create()
    {
        if (!self) {
            self = new AnyTypeBodyPartFormatter();
        }
        return self;
    }
};

const AnyTypeBodyPartFormatter *AnyTypeBodyPartFormatter::self = 0;

class ImageTypeBodyPartFormatter
    : public MessageViewer::Interface::BodyPartFormatter
{
    static const ImageTypeBodyPartFormatter *self;
public:
    Result format(Interface::BodyPart *, Interface::HtmlWriter *) const Q_DECL_OVERRIDE
    {
        return AsIcon;
    }

    // unhide the overload with three arguments
    using MessageViewer::Interface::BodyPartFormatter::format;

    bool process(ObjectTreeParser *, KMime::Content *, ProcessResult &result) const
    {
        result.setIsImage(true);
        return false;
    }
    static const MessageViewer::Interface::BodyPartFormatter *create()
    {
        if (!self) {
            self = new ImageTypeBodyPartFormatter();
        }
        return self;
    }
};

const ImageTypeBodyPartFormatter *ImageTypeBodyPartFormatter::self = 0;

class MessageRfc822BodyPartFormatter
    : public MessageViewer::Interface::BodyPartFormatter
{
    static const MessageRfc822BodyPartFormatter *self;
public:
    Interface::MessagePart::Ptr process(Interface::BodyPart &) const Q_DECL_OVERRIDE;
    MessageViewer::Interface::BodyPartFormatter::Result format(Interface::BodyPart *, Interface::HtmlWriter *) const Q_DECL_OVERRIDE;
    using MessageViewer::Interface::BodyPartFormatter::format;
    static const MessageViewer::Interface::BodyPartFormatter *create();
};

const MessageRfc822BodyPartFormatter *MessageRfc822BodyPartFormatter::self;

const MessageViewer::Interface::BodyPartFormatter *MessageRfc822BodyPartFormatter::create()
{
    if (!self) {
        self = new MessageRfc822BodyPartFormatter();
    }
    return self;
}

Interface::MessagePart::Ptr MessageRfc822BodyPartFormatter::process(Interface::BodyPart &part) const
{
    const KMime::Message::Ptr message = part.content()->bodyAsMessage();
    return MessagePart::Ptr(new EncapsulatedRfc822MessagePart(part.objectTreeParser(), part.content(), message));
}

Interface::BodyPartFormatter::Result MessageRfc822BodyPartFormatter::format(Interface::BodyPart *part, Interface::HtmlWriter *writer) const
{
    Q_UNUSED(writer)
    const ObjectTreeParser *otp = part->objectTreeParser();
    const auto p = process(*part);
    const auto mp =  static_cast<MessagePart *>(p.data());
    if (mp && !otp->attachmentStrategy()->inlineNestedMessages() && !otp->showOnlyOneMimePart()) {
        return Failed;
    } else {
        mp->html(true);
        return Ok;
    }
}

#define CREATE_BODY_PART_FORMATTER(subtype) \
    class subtype##BodyPartFormatter \
        : public MessageViewer::Interface::BodyPartFormatter \
    { \
        static const subtype##BodyPartFormatter *self; \
    public: \
        Interface::MessagePart::Ptr process(Interface::BodyPart &part) const Q_DECL_OVERRIDE; \
        MessageViewer::Interface::BodyPartFormatter::Result format(Interface::BodyPart *, Interface::HtmlWriter *) const Q_DECL_OVERRIDE; \
        using MessageViewer::Interface::BodyPartFormatter::format; \
        static const MessageViewer::Interface::BodyPartFormatter *create(); \
    }; \
    \
    const subtype##BodyPartFormatter *subtype##BodyPartFormatter::self; \
    \
    const MessageViewer::Interface::BodyPartFormatter *subtype##BodyPartFormatter::create() { \
        if ( !self ) { \
            self = new subtype##BodyPartFormatter(); \
        } \
        return self; \
    } \
    Interface::MessagePart::Ptr subtype##BodyPartFormatter::process(Interface::BodyPart &part) const { \
        return part.objectTreeParser()->process##subtype##Subtype(part.content(), *part.processResult()); \
    } \
    \
    MessageViewer::Interface::BodyPartFormatter::Result subtype##BodyPartFormatter::format(Interface::BodyPart *part, Interface::HtmlWriter *writer) const { \
        Q_UNUSED(writer) \
        const auto p = process(*part);\
        const auto mp = static_cast<MessagePart *>(p.data());\
        if (mp) { \
            mp->html(false);\
            return Ok;\
        }\
        return Failed;\
    }

CREATE_BODY_PART_FORMATTER(TextPlain)
CREATE_BODY_PART_FORMATTER(Mailman)
CREATE_BODY_PART_FORMATTER(TextHtml)

CREATE_BODY_PART_FORMATTER(ApplicationPkcs7Mime)

CREATE_BODY_PART_FORMATTER(MultiPartMixed)
CREATE_BODY_PART_FORMATTER(MultiPartAlternative)
CREATE_BODY_PART_FORMATTER(MultiPartSigned)
CREATE_BODY_PART_FORMATTER(MultiPartEncrypted)

typedef TextPlainBodyPartFormatter ApplicationPgpBodyPartFormatter;

#undef CREATE_BODY_PART_FORMATTER
} // anon namespace

// FIXME: port some more BodyPartFormatters to Interface::BodyPartFormatters
void BodyPartFormatterFactoryPrivate::messageviewer_create_builtin_bodypart_formatters(BodyPartFormatterFactoryPrivate::TypeRegistry *reg)
{
    if (!reg) {
        return;
    }
    (*reg)["application"].insert(std::make_pair("octet-stream", AnyTypeBodyPartFormatter::create()));
    (*reg)["application"].insert(std::make_pair("pgp", ApplicationPgpBodyPartFormatter::create()));
    (*reg)["application"].insert(std::make_pair("pkcs7-mime", ApplicationPkcs7MimeBodyPartFormatter::create()));
    (*reg)["application"].insert(std::make_pair("x-pkcs7-mime", ApplicationPkcs7MimeBodyPartFormatter::create()));
    (*reg)["application"].insert(std::make_pair("*", AnyTypeBodyPartFormatter::create()));

    (*reg)["text"].insert(std::make_pair("html", TextHtmlBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("rtf", AnyTypeBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("vcard", AnyTypeBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("x-vcard", AnyTypeBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("plain", MailmanBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("plain", TextPlainBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("*", MailmanBodyPartFormatter::create()));
    (*reg)["text"].insert(std::make_pair("*", TextPlainBodyPartFormatter::create()));

    (*reg)["image"].insert(std::make_pair("*", ImageTypeBodyPartFormatter::create()));

    (*reg)["message"].insert(std::make_pair("rfc822", MessageRfc822BodyPartFormatter::create()));
    (*reg)["message"].insert(std::make_pair("*", AnyTypeBodyPartFormatter::create()));

    (*reg)["multipart"].insert(std::make_pair("alternative", MultiPartAlternativeBodyPartFormatter::create()));
    (*reg)["multipart"].insert(std::make_pair("encrypted", MultiPartEncryptedBodyPartFormatter::create()));
    (*reg)["multipart"].insert(std::make_pair("signed", MultiPartSignedBodyPartFormatter::create()));
    (*reg)["multipart"].insert(std::make_pair("*", MultiPartMixedBodyPartFormatter::create()));
    (*reg)["*"].insert(std::make_pair("*", AnyTypeBodyPartFormatter::create()));
}
