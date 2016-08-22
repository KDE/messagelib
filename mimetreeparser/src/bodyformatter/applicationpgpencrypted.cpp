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

#include "applicationpgpencrypted.h"

#include "utils.h"

#include "viewer/objecttreeparser.h"
#include "viewer/messagepart.h"

#include <Libkleo/CryptoBackendFactory>

#include <KMime/Content>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const ApplicationPGPEncryptedBodyPartFormatter *ApplicationPGPEncryptedBodyPartFormatter::self;

const Interface::BodyPartFormatter *ApplicationPGPEncryptedBodyPartFormatter::create()
{
    if (!self) {
        self = new ApplicationPGPEncryptedBodyPartFormatter();
    }
    return self;
}

Interface::BodyPartFormatter::Result ApplicationPGPEncryptedBodyPartFormatter::format(Interface::BodyPart *part, HtmlWriter *writer) const
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

Interface::MessagePart::Ptr ApplicationPGPEncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node(part.content());

    if (node->decodedContent().trimmed() != "Version: 1") {
        qCWarning(MIMETREEPARSER_LOG) << "Unknown PGP Version String:" << node->decodedContent().trimmed();
    }

    if (!part.content()->parent()) {
        return MessagePart::Ptr();
    }

    KMime::Content *data = findTypeInDirectChilds(part.content()->parent(), "application/octet-stream");

    if (!data) {
        return MessagePart::Ptr(); //new MimeMessagePart(part.objectTreeParser(), node, false));
    }

    part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);

    EncryptedMessagePart::Ptr mp(new EncryptedMessagePart(part.objectTreeParser(),
                                 data->decodedText(), Kleo::CryptoBackendFactory::instance()->openpgp(),
                                 NodeHelper::fromAsString(data), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());
    if (!part.source()->decryptMessage()) {
        part.nodeHelper()->setNodeProcessed(data, false);  // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = part.nodeHelper()->decryptedNodeForContent(data)) {
        // if we already have a decrypted node for this encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, part.objectTreeParser()->showOnlyOneMimePart()));
    } else {
        mp->startDecryption(data);
        if (!messagePart->inProgress) {
            part.nodeHelper()->setNodeProcessed(data, false);   // Set the data node to done to prevent it from being processed
            if (messagePart->isDecryptable && messagePart->isSigned) {
                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
            }
        }
    }
    return mp;
}
