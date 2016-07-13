/*
   Copyright (c) 2016 Sandro Knauß <sknauss@kde.org>

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

#include "multipartsigned.h"

#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <KMime/Content>

#include <Libkleo/CryptoBackendFactory>

#include "mimetreeparser_debug.h"

#include <QTextCodec>

using namespace MimeTreeParser;

const MultiPartSignedBodyPartFormatter *MultiPartSignedBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartSignedBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartSignedBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result MultiPartSignedBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
{
    Q_UNUSED(writer)
    const auto p = process(*part);
    const auto mp = static_cast<MessagePart *>(p.data());
    if (mp) {
        mp->html(false);
        return Ok;
    }
    return Failed;
}

Interface::MessagePart::Ptr MultiPartSignedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    if (node->contents().size() != 2) {
        qCDebug(MIMETREEPARSER_LOG) << "mulitpart/signed must have exactly two child parts!" << endl
                                    << "processing as multipart/mixed";
        if (!node->contents().isEmpty()) {
            return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), node->contents().at(0), false));
        } else {
            return MessagePart::Ptr();
        }
    }

    KMime::Content *signedData =  node->contents().at(0);
    KMime::Content *signature = node->contents().at(1);
    assert(signedData);
    assert(signature);

    QString protocolContentType = node->contentType()->parameter(QStringLiteral("protocol")).toLower();
    const QString signatureContentType = QLatin1String(signature->contentType()->mimeType().toLower());
    if (protocolContentType.isEmpty()) {
        qCWarning(MIMETREEPARSER_LOG) << "Message doesn't set the protocol for the multipart/signed content-type, "
                                      "using content-type of the signature:" << signatureContentType;
        protocolContentType = signatureContentType;
    }

    const Kleo::CryptoBackend::Protocol *protocol = Q_NULLPTR;
    if (protocolContentType == QLatin1String("application/pkcs7-signature") ||
            protocolContentType == QLatin1String("application/x-pkcs7-signature")) {
        protocol = Kleo::CryptoBackendFactory::instance()->smime();
    } else if (protocolContentType == QLatin1String("application/pgp-signature") ||
               protocolContentType == QLatin1String("application/x-pgp-signature")) {
        protocol = Kleo::CryptoBackendFactory::instance()->openpgp();
    }

    if (!protocol) {
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), signedData, false));
    }

    part.nodeHelper()->setNodeProcessed(signature, true);

    part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);

    const QByteArray cleartext = KMime::LFtoCRLF(signedData->encodedContent());
    const QTextCodec *aCodec(part.objectTreeParser()->codecFor(signedData));

    SignedMessagePart::Ptr mp(new SignedMessagePart(part.objectTreeParser(),
                              aCodec->toUnicode(cleartext), protocol,
                              NodeHelper::fromAsString(node), signature));
    PartMetaData *messagePart(mp->partMetaData());

    if (protocol) {
        mp->startVerificationDetached(cleartext, signedData, signature->decodedContent());
    } else {
        messagePart->auditLogError = GpgME::Error(GPG_ERR_NOT_IMPLEMENTED);
    }

    return mp;
}
