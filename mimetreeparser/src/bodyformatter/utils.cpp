/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "utils.h"

using namespace MimeTreeParser;

MimeMessagePart::Ptr MimeTreeParser::createAndParseTempNode(Interface::BodyPart &part, KMime::Content *parentNode, const char *content, const char *cntDesc)
{
    auto newNode = new KMime::Content();
    newNode->setContent(KMime::CRLFtoLF(content));
    newNode->parse();

    if (!newNode->head().isEmpty()) {
        newNode->contentDescription()->from7BitString(cntDesc);
    }
    part.nodeHelper()->attachExtraContent(parentNode, newNode);

    return MimeMessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, false));
}

KMime::Content *MimeTreeParser::findTypeInDirectChilds(KMime::Content *content, const QByteArray &mimeType)
{
    if (mimeType.isEmpty()) {
        return content;
    }

    const auto contents = content->contents();
    for (const auto child : contents) {
        if ((!child->contentType()->isEmpty()) && (mimeType == child->contentType(false)->mimeType())) {
            return child;
        }
    }
    return nullptr;
}

MessagePart::Ptr MimeTreeParser::toplevelTextNode(MessagePart::Ptr messageTree)
{
    const auto subParts = messageTree->subParts();
    for (const auto &mp : subParts) {
        auto text = mp.dynamicCast<TextMessagePart>();
        auto attach = mp.dynamicCast<AttachmentMessagePart>();
        if (text && !attach) {
            return text;
        } else if (const auto alternative = mp.dynamicCast<AlternativeMessagePart>()) {
            return alternative;
        } else {
            auto ret = toplevelTextNode(mp);
            if (ret) {
                return ret;
            }
        }
    }
    return {};
}
