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

#include "multipartencrypted.h"

#include "utils.h"

#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <KMime/Content>

#include <Libkleo/CryptoBackendFactory>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const MultiPartEncryptedBodyPartFormatter *MultiPartEncryptedBodyPartFormatter::self;

const Interface::BodyPartFormatter *MultiPartEncryptedBodyPartFormatter::create()
{
    if (!self) {
        self = new MultiPartEncryptedBodyPartFormatter();
    }
    return self;
}
Interface::BodyPartFormatter::Result MultiPartEncryptedBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr MultiPartEncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    if (node->contents().isEmpty()) {
        Q_ASSERT(false);
        return MessagePart::Ptr();
    }

    const Kleo::CryptoBackend::Protocol *useThisCryptProto = Q_NULLPTR;

    /*
    ATTENTION: This code is to be replaced by the new 'auto-detect' feature. --------------------------------------
    */
    KMime::Content *data = findTypeInDirectChilds(node, "application/octet-stream");
    if (data) {
        useThisCryptProto = Kleo::CryptoBackendFactory::instance()->openpgp();
    }
    if (!data) {
        data = findTypeInDirectChilds(node, "application/pkcs7-mime");
        if (data) {
            useThisCryptProto = Kleo::CryptoBackendFactory::instance()->smime();
        }
    }
    /*
    ---------------------------------------------------------------------------------------------------------------
    */

    if (!data) {
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), node->contents().at(0), false));
    }

    part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);

    CryptoMessagePart::Ptr mp(new CryptoMessagePart(part.objectTreeParser(),
                              data->decodedText(), useThisCryptProto,
                              NodeHelper::fromAsString(data), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());
    if (!part.source()->decryptMessage()) {
        part.nodeHelper()->setNodeProcessed(data, false);  // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = part.nodeHelper()->decryptedNodeForContent(data)) {
        // if we already have a decrypted node for part.objectTreeParser() encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, part.objectTreeParser()->showOnlyOneMimePart()));
    } else {
        mp->startDecryption(data);

        qCDebug(MIMETREEPARSER_LOG) << "decrypted, signed?:" << messagePart->isSigned;

        if (!messagePart->inProgress) {
            part.nodeHelper()->setNodeProcessed(data, false);   // Set the data node to done to prevent it from being processed
            if (messagePart->isDecryptable && messagePart->isSigned) {
                // Note: Multipart/Encrypted might also be signed
                //       without encapsulating a nicely formatted
                //       ~~~~~~~                 Multipart/Signed part.
                //                               (see RFC 3156 --> 6.2)
                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
            }
        }
    }
    return mp;
}
