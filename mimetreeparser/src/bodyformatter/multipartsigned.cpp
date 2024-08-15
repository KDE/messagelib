/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartsigned.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

#include <QGpgME/Protocol>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const MultiPartSignedBodyPartFormatter *MultiPartSignedBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartSignedBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartSignedBodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr MultiPartSignedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    if (node->contents().size() != 2) {
        qCDebug(MIMETREEPARSER_LOG) << "multipart/signed must have exactly two child parts!" << Qt::endl << "processing as multipart/mixed";
        if (!node->contents().isEmpty()) {
            return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), node->contents().at(0), false));
        } else {
            return {};
        }
    }

    KMime::Content *signedData = node->contents().at(0);
    KMime::Content *signature = node->contents().at(1);
    Q_ASSERT(signedData);
    Q_ASSERT(signature);

    QString protocolContentType = node->contentType()->parameter("protocol").toLower();
    const QString signatureContentType = QLatin1StringView(signature->contentType()->mimeType().toLower());
    if (protocolContentType.isEmpty()) {
        qCWarning(MIMETREEPARSER_LOG) << "Message doesn't set the protocol for the multipart/signed content-type, "
                                         "using content-type of the signature:"
                                      << signatureContentType;
        protocolContentType = signatureContentType;
    }

    const QGpgME::Protocol *protocol = nullptr;
    if (protocolContentType == QLatin1StringView("application/pkcs7-signature") || protocolContentType == QLatin1StringView("application/x-pkcs7-signature")) {
        protocol = QGpgME::smime();
    } else if (protocolContentType == QLatin1StringView("application/pgp-signature")
               || protocolContentType == QLatin1StringView("application/x-pgp-signature")) {
        protocol = QGpgME::openpgp();
    }

    if (!protocol) {
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), signedData, false));
    }

    part.nodeHelper()->setNodeProcessed(signature, true);

    part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);

    const QByteArray cleartext = KMime::LFtoCRLF(signedData->encodedContent());
    QStringDecoder aCodec(part.objectTreeParser()->codecNameFor(signedData).constData());

    SignedMessagePart::Ptr mp(
        new SignedMessagePart(part.objectTreeParser(), aCodec.decode(cleartext), protocol, part.nodeHelper()->fromAsString(node), signedData));

    mp->startVerificationDetached(cleartext, signedData, signature->decodedContent());
    part.nodeHelper()->registerOverrideHeader(node, mp);
    return mp;
}
