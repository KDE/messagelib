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

#include "mimetreeparser_debug.h"

#include "bodyformatter/applicationpgpencrypted.h"
#include "bodyformatter/applicationpkcs7mime.h"
#include "bodyformatter/mailman.h"
#include "bodyformatter/multipartalternative.h"
#include "bodyformatter/multipartmixed.h"
#include "bodyformatter/multipartencrypted.h"
#include "bodyformatter/multipartsigned.h"
#include "bodyformatter/texthtml.h"
#include "bodyformatter/textplain.h"

#include "interfaces/bodypartformatter.h"
#include "interfaces/bodypart.h"
#include "interfaces/htmlwriter.h"

#include "bodypartformatterfactory.h"
#include "bodypartformatterfactory_p.h"

#include "attachmentstrategy.h"
#include "objecttreeparser.h"
#include "messagepart.h"

#include <KMime/Content>

#include <QUrl>

using namespace MimeTreeParser;

namespace {
class AnyTypeBodyPartFormatter : public MimeTreeParser::Interface::BodyPartFormatter
{
    static const AnyTypeBodyPartFormatter *self;
public:
    MessagePart::Ptr process(Interface::BodyPart &part) const override
    {
        KMime::Content *node = part.content();
        const auto mp = AttachmentMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, false, part.source()->decryptMessage()));
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
        auto mp = AttachmentMessagePart::Ptr(new AttachmentMessagePart(part.objectTreeParser(), node, false, part.source()->decryptMessage()));
        mp->setIsImage(true);
        part.processResult()->setInlineSignatureState(mp->signatureState());
        part.processResult()->setInlineEncryptionState(mp->encryptionState());

        auto preferredMode = part.source()->preferredMode();
        bool isHtmlPreferred = (preferredMode == Util::Html) || (preferredMode == Util::MultipartHtml);
        if (node->parent() && node->parent()->contentType()->subType() == "related" && isHtmlPreferred && !part.objectTreeParser()->showOnlyOneMimePart()) {
            QString fileName = mp->temporaryFilePath();
            QString href = QUrl::fromLocalFile(fileName).url();
            QByteArray cid = node->contentID()->identifier();
            if (part.objectTreeParser()->htmlWriter()) {
                part.objectTreeParser()->htmlWriter()->embedPart(cid, href);
            }
            part.nodeHelper()->setNodeDisplayedEmbedded(node, true);
            part.nodeHelper()->setNodeDisplayedHidden(node, true);
            return mp;
        }

        // Show it inline if showOnlyOneMimePart(), which means the user clicked the image
        // in the message structure viewer manually, and therefore wants to see the full image
        if (part.objectTreeParser()->showOnlyOneMimePart() && !part.processResult()->neverDisplayInline()) {
            part.nodeHelper()->setNodeDisplayedEmbedded(node, true);
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
    insert(QStringLiteral("application/octet-stream"), AnyTypeBodyPartFormatter::create());

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
