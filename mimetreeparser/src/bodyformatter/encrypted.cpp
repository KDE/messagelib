/*
   Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

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

#include "encrypted.h"

#include "utils.h"

#include "objecttreeparser.h"
#include "messagepart.h"

#include <KMime/Content>

#include <QGpgME/Protocol>
#include <QGpgME/DataProvider>
#include <gpgme++/data.h>

#include <QTextCodec>

#include "mimetreeparser_debug.h"

using namespace MimeTreeParser;

const EncryptedBodyPartFormatter *EncryptedBodyPartFormatter::self;

const Interface::BodyPartFormatter *EncryptedBodyPartFormatter::create()
{
    if (!self) {
        self = new EncryptedBodyPartFormatter();
    }
    return self;
}

MessagePart::Ptr EncryptedBodyPartFormatter::process(Interface::BodyPart &part) const
{
    KMime::Content *node = part.content();

    if (!node->contents().isEmpty()) {
        Q_ASSERT(false);
        return MessagePart::Ptr();
    }

    QGpgME::QByteArrayDataProvider dp(node->decodedContent());
    GpgME::Data data(&dp);

    if (data.type() == GpgME::Data::Unknown) {
        return nullptr;
    }

    const QGpgME::Protocol *useThisCryptProto = nullptr;

    useThisCryptProto = QGpgME::openpgp();

    //TODO: Load correct crypto Proto

    part.nodeHelper()->setEncryptionState(node, KMMsgFullyEncrypted);

    EncryptedMessagePart::Ptr mp(new EncryptedMessagePart(part.objectTreeParser(),
                                                          node->decodedText(), useThisCryptProto,
                                                          part.nodeHelper()->fromAsString(node), node));
    mp->setIsEncrypted(true);
    mp->setDecryptMessage(part.source()->decryptMessage());
    PartMetaData *messagePart(mp->partMetaData());
    if (!part.source()->decryptMessage()) {
        part.nodeHelper()->setNodeProcessed(node, false);  // Set the data node to done to prevent it from being processed
    } else if (KMime::Content *newNode = part.nodeHelper()->decryptedNodeForContent(node)) {
        // if we already have a decrypted node for part.objectTreeParser() encrypted node, don't do the decryption again
        return MessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, true));
    } else {
        const auto codec = QTextCodec::codecForName("utf-8");
        mp->startDecryption(node->decodedContent(), codec);

        qCDebug(MIMETREEPARSER_LOG) << "decrypted, signed?:" << messagePart->isSigned;

        if (!messagePart->inProgress) {
            if (!messagePart->isEncrypted) {
                return nullptr;
            }
            auto tempNode = new KMime::Content();
            tempNode->contentType()->setCharset("utf-8");
            tempNode->setBody(KMime::CRLFtoLF(part.nodeHelper()->codec(node)->fromUnicode(mp->text())));
            tempNode->parse();
            NodeHelper::magicSetType(tempNode);
            if (node->topLevel()->textContent() != node && node->contentDisposition(false) && !tempNode->contentDisposition(false)) {
                tempNode->contentDisposition()->setDisposition(node->contentDisposition()->disposition());
                const auto fname = node->contentDisposition()->filename();
                if (!fname.isEmpty()) {
                    tempNode->contentDisposition()->setFilename(fname);
                }
            }

            if (!tempNode->head().isEmpty()) {
                tempNode->contentDescription()->from7BitString("decrypted data");
            }
            tempNode->assemble();

            part.nodeHelper()->cleanExtraContent(node);
            mp->clearSubParts();

            part.nodeHelper()->attachExtraContent(node, tempNode);

            mp->parseInternal(tempNode, false);

            part.nodeHelper()->setNodeProcessed(node, false);   // Set the data node to done to prevent it from being processed
        }
    }
    return mp;
}
