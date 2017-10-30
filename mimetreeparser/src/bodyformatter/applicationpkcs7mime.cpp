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

#include "utils.h"

#include "objecttreeparser.h"
#include "messagepart.h"

#include <QGpgME/Protocol>

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

MessagePart::Ptr ApplicationPkcs7MimeBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();

    if (node->head().isEmpty()) {
        return MessagePart::Ptr();
    }

    const auto smimeCrypto = QGpgME::smime();
    if (!smimeCrypto) {
        return MessagePart::Ptr();
    }

    // we are also registered for octet-stream, in that case stop here if that's not a part for us
    const auto mt = node->contentType()->mimeType();
    const auto isCorrectMimeType = mt == QByteArrayLiteral("application/pkcs7-mime") || mt == QByteArrayLiteral("application/x-pkcs7-mime");
    const auto hasCorrectName = mt == QByteArrayLiteral("application/octet-stream")
                                && (node->contentType()->name().endsWith(QLatin1String("p7m"))
                                    || node->contentType()->name().endsWith(QLatin1String("p7s"))
                                    || node->contentType()->name().endsWith(QLatin1String("p7c")));
    if (!isCorrectMimeType && !hasCorrectName) {
        return {};
    }

    const QString smimeType = node->contentType()->parameter(QStringLiteral("smime-type")).toLower();

    if (smimeType == QLatin1String("certs-only")) {
        part.processResult()->setNeverDisplayInline(true);

        CertMessagePart::Ptr mp(new CertMessagePart(part.objectTreeParser(), node, smimeCrypto, part.source()->autoImportKeys()));
        return mp;
    }

    bool isSigned = (smimeType == QLatin1String("signed-data"));
    bool isEncrypted = (smimeType == QLatin1String("enveloped-data"));

    // Analyze "signTestNode" node to find/verify a signature.
    // If zero part.objectTreeParser() verification was successfully done after
    // decrypting via recursion by insertAndParseNewChildNode().
    KMime::Content *signTestNode = isEncrypted ? nullptr : node;

    // We try decrypting the content
    // if we either *know* that it is an encrypted message part
    // or there is neither signed nor encrypted parameter.
    MessagePart::Ptr mp;
    if (!isSigned) {
        if (isEncrypted) {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime     ==      S/MIME TYPE: enveloped (encrypted) data";
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  type unknown  -  enveloped (encrypted) data ?";
        }

        auto _mp = EncryptedMessagePart::Ptr(new EncryptedMessagePart(part.objectTreeParser(),
                                                                      node->decodedText(), smimeCrypto,
                                                                      part.nodeHelper()->fromAsString(node), node));
        mp = _mp;
        _mp->setIsEncrypted(true);
        _mp->setDecryptMessage(part.source()->decryptMessage());
        PartMetaData *messagePart(_mp->partMetaData());
        if (!part.source()->decryptMessage()) {
            isEncrypted = true;
            signTestNode = nullptr; // PENDING(marc) to be abs. sure, we'd need to have to look at the content
        } else {
            _mp->startDecryption();
            if (messagePart->isDecryptable) {
                qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  encryption found  -  enveloped (encrypted) data !";
                isEncrypted = true;
                part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);
                signTestNode = nullptr;
            } else {
                // decryption failed, which could be because the part was encrypted but
                // decryption failed, or because we didn't know if it was encrypted, tried,
                // and failed. If the message was not actually encrypted, we continue
                // assuming it's signed
                if (_mp->passphraseError() || (smimeType.isEmpty() && messagePart->isEncrypted)) {
                    isEncrypted = true;
                    signTestNode = nullptr;
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
        auto _mp = SignedMessagePart::Ptr(new SignedMessagePart(part.objectTreeParser(),
                                                                aCodec->toUnicode(signaturetext), smimeCrypto,
                                                                part.nodeHelper()->fromAsString(node), signTestNode));
        mp = _mp;
        _mp->startVerificationDetached(signaturetext, nullptr, QByteArray());

        if (_mp->isSigned()) {
            if (!isSigned) {
                qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  signature found  -  opaque signed data !";
            }

            if (signTestNode != node) {
                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
            }
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "pkcs7 mime  -  NO signature found   :-(";
        }
    }

    return mp;
}
