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

#include "utils.h"

#include <MessageCore/NodeHelper>

using namespace MessageViewer;

MimeMessagePart::Ptr MessageViewer::createAndParseTempNode(Interface::BodyPart &part, KMime::Content *parentNode, const char *content, const char *cntDesc)
{
    KMime::Content *newNode = new KMime::Content();
    newNode->setContent(KMime::CRLFtoLF(content));
    newNode->parse();

    if (!newNode->head().isEmpty()) {
        newNode->contentDescription()->from7BitString(cntDesc);
    }
    part.nodeHelper()->attachExtraContent(parentNode, newNode);

    return MimeMessagePart::Ptr(new MimeMessagePart(part.objectTreeParser(), newNode, false));
}

KMime::Content *MessageViewer::findType(KMime::Content *content, const QByteArray &mimeType, bool deep, bool wide)
{
    if ((!content->contentType()->isEmpty())
            && (mimeType.isEmpty()  || (mimeType == content->contentType()->mimeType()))) {
        return content;
    }
    KMime::Content *child = MessageCore::NodeHelper::firstChild(content);
    if (child && deep) { //first child
        return findType(child, mimeType, deep, wide);
    }

    KMime::Content *next = MessageCore::NodeHelper::nextSibling(content);
    if (next &&  wide) { //next on the same level
        return findType(next, mimeType, deep, wide);
    }

    return Q_NULLPTR;
}