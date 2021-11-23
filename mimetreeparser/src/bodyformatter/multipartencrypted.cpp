/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "multipartencrypted.h"

#include "utils.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

#include <QGpgME/Protocol>

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

MessagePart::Ptr MultiPartEncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();

    if (node->contents().isEmpty()) {
        Q_ASSERT(false);
        return {};
    }

    const QGpgME::Protocol *useThisCryptProto = nullptr;

    /*
    ATTENTION: This code is to be replaced by the new 'auto-detect' feature. --------------------------------------
    */
    KMime::Content *data = findTypeInDirectChilds(node, "application/octet-stream");
    if (data) {
        useThisCryptProto = QGpgME::openpgp();
    }
    if (!data) {
        data = findTypeInDirectChilds(node, "application/pkcs7-mime");
        if (data) {
            useThisCryptProto = QGpgME::smime();
        }
    }
    /*
    ---------------------------------------------------------------------------------------------------------------
    */

    if (!data) {
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), node->contents().at(0), false));
    }

    part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);

    EncryptedMessagePart::Ptr mp(
        new EncryptedMessagePart(part.objectTreeParser(), data->decodedText(), useThisCryptProto, part.nodeHelper()->fromAsString(data), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());

    if (!part.source()->decryptMessage()) {
        part.nodeHelper()->setNodeProcessed(data, false); // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = part.nodeHelper()->decryptedNodeForContent(data)) {
        part.nodeHelper()->registerOverrideHeader(data->parent(), mp);

        // if we already have a decrypted node for part.objectTreeParser() encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, true));
    } else {
        mp->startDecryption(data);
        qCDebug(MIMETREEPARSER_LOG) << "decrypted, signed?:" << messagePart->isSigned;

        if (!messagePart->inProgress) {
            part.nodeHelper()->registerOverrideHeader(data->parent(), mp);
            part.nodeHelper()->setNodeProcessed(data, false); // Set the data node to done to prevent it from being processed
        }
    }
    return mp;
}
