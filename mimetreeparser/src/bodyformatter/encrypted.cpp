/*
   SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "encrypted.h"

#include "utils.h"

#include "messagepart.h"
#include "objecttreeparser.h"

#include <KMime/Content>

#include <QGpgME/DataProvider>
#include <QGpgME/Protocol>
#include <gpgme++/data.h>

#include <QTextCodec>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const Interface::BodyPartFormatter *EncryptedBodyPartFormatter::create(EncryptedBodyPartFormatter::EncryptionFlags flags)
{
    auto self = new EncryptedBodyPartFormatter;
    self->mFlags = flags;
    return self;
}

MessagePart::Ptr EncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();
    const auto nodeHelper = part.nodeHelper();

    if (!node->contents().isEmpty()) {
        Q_ASSERT(false);
        return {};
    }

    const QByteArray content(node->decodedContent());
    if (content.isEmpty()) {
        return nullptr;
    }

    if (!(mFlags & EncryptedBodyPartFormatter::ForcePGP)) {
        // If not forcing the data to be interpreted as PGP encrypted,
        // only check for encryption if it starts with a 7-bit ASCII
        // character.  Valid armored PGP data always starts with an
        // ASCII character, so if the first byte has bit 8 set then it
        // cannot be PGP armored.  This way we retain support for armored
        // inline PGP data, but avoid random binary data being detected
        // as PGP data.  See bug 390002 and messagelib!83.
        unsigned char firstByte = content[0];
        if ((firstByte & 0x80) != 0) {
            return nullptr;
        }

        QGpgME::QByteArrayDataProvider dp(content);
        GpgME::Data data(&dp);

        if (data.type() == GpgME::Data::Unknown) {
            return nullptr;
        }
    }

    const QGpgME::Protocol *useThisCryptProto = nullptr;

    useThisCryptProto = QGpgME::openpgp();

    // TODO: Load correct crypto Proto

    nodeHelper->setEncryptionState(node, KMMsgFullyEncrypted);

    EncryptedMessagePart::Ptr mp(
        new EncryptedMessagePart(part.objectTreeParser(), node->decodedText(), useThisCryptProto, nodeHelper->fromAsString(node), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());

    if (!part.source()->decryptMessage()) {
        nodeHelper->setNodeProcessed(node, false); // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = nodeHelper->decryptedNodeForContent(node)) {
        // if we already have a decrypted node for part.objectTreeParser() encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, true));
    } else {
        const auto codec = part.nodeHelper()->codec(node);
        mp->startDecryption(node->decodedContent(), codec);

        qCDebug(MIMETREEPARSER_LOG) << "decrypted, signed?:" << messagePart->isSigned;

        if (!messagePart->inProgress) {
            if (!messagePart->isEncrypted) {
                return nullptr;
            }
            auto tempNode = new KMime::Content();
            tempNode->setHead("Content-Type: text/plain; charset=\"" + codec->name() + "\"");
            tempNode->setBody(KMime::CRLFtoLF(codec->fromUnicode(mp->text())));
            tempNode->parse();
            NodeHelper::magicSetType(tempNode);
            if (node->topLevel()->textContent() != node && node->contentDisposition(false) && !tempNode->contentDisposition(false)) {
                tempNode->contentDisposition()->setDisposition(node->contentDisposition()->disposition());
                const auto fname = node->contentDisposition(false)->filename();
                if (!fname.isEmpty()) {
                    tempNode->contentDisposition(false)->setFilename(fname);
                }
            }

            if (!tempNode->head().isEmpty()) {
                tempNode->contentDescription()->from7BitString("decrypted data");
            }
            tempNode->assemble();

            nodeHelper->cleanExtraContent(node);
            mp->clearSubParts();

            nodeHelper->attachExtraContent(node, tempNode);

            mp->parseInternal(tempNode, false);

            nodeHelper->setNodeProcessed(node, false); // Set the data node to done to prevent it from being processed
        }
    }
    return mp;
}
