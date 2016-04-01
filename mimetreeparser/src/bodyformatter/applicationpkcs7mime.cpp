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

#include "applicationpkcs7mime.h"

#include "viewer/attachmentstrategy.h"
#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <Libkleo/CryptoBackendFactory>

#include <KMime/Content>

#include <QTextCodec>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const ApplicationPkcs7MimeBodyPartFormatter *ApplicationPkcs7MimeBodyPartFormatter::self;

const Interface::BodyPartFormatter *ApplicationPkcs7MimeBodyPartFormatter::create()
{
    if (!self) {
        self = new ApplicationPkcs7MimeBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result ApplicationPkcs7MimeBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr ApplicationPkcs7MimeBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();

    if (node->head().isEmpty()) {
        return MessagePart::Ptr();
    }

    const Kleo::CryptoBackend::Protocol *smimeCrypto = Kleo::CryptoBackendFactory::instance()->smime();
    if (!smimeCrypto) {
        return MessagePart::Ptr();
    }

    const QString smimeType = node->contentType()->parameter(QStringLiteral("smime-type")).toLower();

    if (smimeType == QLatin1String("certs-only")) {
        part.processResult()->setNeverDisplayInline(true);

        CertMessagePart::Ptr mp(new CertMessagePart(part.objectTreeParser(), node, smimeCrypto, part.source()->autoImportKeys()));
        return mp;
    }

    bool isSigned      = (smimeType == QLatin1String("signed-data"));
    bool isEncrypted   = (smimeType == QLatin1String("enveloped-data"));

    // Analyze "signTestNode" node to find/verify a signature.
    // If zero part.objectTreeParser() verification was successfully done after
    // decrypting via recursion by insertAndParseNewChildNode().
    KMime::Content *signTestNode = isEncrypted ? 0 : node;

    // We try decrypting the content
    // if we either *know* that it is an encrypted message part
    // or there is neither signed nor encrypted parameter.
    CryptoMessagePart::Ptr mp;
    if (!isSigned) {
        if (isEncrypted) {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime     ==      S/MIME TYPE: enveloped (encrypted) data";
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  type unknown  -  enveloped (encrypted) data ?";
        }

        mp = CryptoMessagePart::Ptr(new CryptoMessagePart(part.objectTreeParser(),
                                    node->decodedText(), smimeCrypto,
                                    NodeHelper::fromAsString(node), node));
        mp->setIsEncrypted(true);
        mp->setDecryptMessage(part.source()->decryptMessage());
        PartMetaData *messagePart(mp->partMetaData());
        if (!part.source()->decryptMessage()) {
            isEncrypted = true;
            signTestNode = 0; // PENDING(marc) to be abs. sure, we'd need to have to look at the content
        } else {
            mp->startDecryption();
            if (messagePart->isDecryptable) {
                qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  encryption found  -  enveloped (encrypted) data !";
                isEncrypted = true;
                part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);
                if (messagePart->isSigned) {
                    part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
                }
                signTestNode = 0;

            } else {
                // decryption failed, which could be because the part was encrypted but
                // decryption failed, or because we didn't know if it was encrypted, tried,
                // and failed. If the message was not actually encrypted, we continue
                // assuming it's signed
                if (mp->passphraseError() || (smimeType.isEmpty() && messagePart->isEncrypted)) {
                    isEncrypted = true;
                    signTestNode = 0;
                }

                if (isEncrypted) {
                    qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  ERROR: COULD NOT DECRYPT enveloped data !";
                } else {
                    qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  NO encryption found";
                }
            }
        }

        if (isEncrypted) {
            part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);
        }
    }

    // We now try signature verification if necessarry.
    if (signTestNode) {
        if (isSigned) {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime     ==      S/MIME TYPE: opaque signed data";
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  type unknown  -  opaque signed data ?";
        }

        const QTextCodec *aCodec(part.objectTreeParser()->codecFor(signTestNode));
        const QByteArray signaturetext = signTestNode->decodedContent();
        mp = CryptoMessagePart::Ptr(new CryptoMessagePart(part.objectTreeParser(),
                                    aCodec->toUnicode(signaturetext), smimeCrypto,
                                    NodeHelper::fromAsString(node), signTestNode));
        mp->setDecryptMessage(part.source()->decryptMessage());
        PartMetaData *messagePart(mp->partMetaData());
        if (smimeCrypto) {
            mp->startVerificationDetached(signaturetext, 0, QByteArray());
        } else {
            messagePart->auditLogError = GpgME::Error(GPG_ERR_NOT_IMPLEMENTED);
        }

        if (messagePart->isSigned) {
            if (!isSigned) {
                qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  signature found  -  opaque signed data !";
                isSigned = true;
            }

            part.nodeHelper()->setSignatureState(signTestNode, KMMsgFullySigned);
            if (signTestNode != node) {
                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
            }
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  NO signature found   :-(";
        }
    }

    return mp;
}
