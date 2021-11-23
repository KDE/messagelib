/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "applicationpgpencrypted.h"

#include "utils.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <QGpgME/Protocol>

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

MessagePart::Ptr ApplicationPGPEncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node(part.content());

    if (node->decodedContent().trimmed() != "Version: 1") {
        qCWarning(MIMETREEPARSER_LOG) << "Unknown PGP Version String:" << node->decodedContent().trimmed();
    }

    if (!part.content()->parent()) {
        return {};
    }

    KMime::Content *data = findTypeInDirectChilds(part.content()->parent(), "application/octet-stream");

    if (!data) {
        return {}; // new MimeMessagePart(part.objectTreeParser(), node, false));
    }

    part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);

    EncryptedMessagePart::Ptr mp(
        new EncryptedMessagePart(part.objectTreeParser(), data->decodedText(), QGpgME::openpgp(), part.nodeHelper()->fromAsString(data), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());
    if (!part.source()->decryptMessage()) {
        part.nodeHelper()->setNodeProcessed(data, false); // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = part.nodeHelper()->decryptedNodeForContent(data)) {
        part.nodeHelper()->registerOverrideHeader(data->parent(), mp);
        // if we already have a decrypted node for this encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, false));
    } else {
        mp->startDecryption(data);
        if (!messagePart->inProgress) {
            part.nodeHelper()->registerOverrideHeader(data->parent(), mp);
            part.nodeHelper()->setNodeProcessed(data, false); // Set the data node to done to prevent it from being processed
            if (messagePart->isDecryptable && messagePart->isSigned) {
                part.nodeHelper()->setSignatureState(node, KMMsgFullySigned);
            }
        }
    }
    return mp;
}
