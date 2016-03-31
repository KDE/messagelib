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

#include <MessageCore/NodeHelper>
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
    KMime::Content *child = MessageCore::NodeHelper::firstChild(node);
    if (!child) {
        Q_ASSERT(false);
        return MessagePart::Ptr();
    }

    const Kleo::CryptoBackend::Protocol *useThisCryptProto = Q_NULLPTR;

    /*
    ATTENTION: This code is to be replaced by the new 'auto-detect' feature. --------------------------------------
    */
    KMime::Content *data = findType(child, "application/octet-stream", false, true);
    if (data) {
        useThisCryptProto = Kleo::CryptoBackendFactory::instance()->openpgp();
    }
    if (!data) {
        data = findType(child, "application/pkcs7-mime", false, true);
        if (data) {
            useThisCryptProto = Kleo::CryptoBackendFactory::instance()->smime();
        }
    }
    /*
    ---------------------------------------------------------------------------------------------------------------
    */

    if (!data) {
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), child, false));
    }

    KMime::Content *dataChild = MessageCore::NodeHelper::firstChild(data);
    if (dataChild) {
        Q_ASSERT(false);
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), dataChild, false));
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
                // In part.objectTreeParser() case we paint a _2nd_ frame inside the
                // encryption frame, but we do _not_ show a respective
                // encapsulated MIME part in the Mime Tree Viewer
                // since we do want to show the _true_ structure of the
                // message there - not the structure that the sender's
                // MUA 'should' have sent.  :-D       (khz, 12.09.2002)

                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
                qCDebug(MIMETREEPARSER_LOG) << "setting FULLY SIGNED to:" << node;
            }
        }
    }
    return mp;
}
