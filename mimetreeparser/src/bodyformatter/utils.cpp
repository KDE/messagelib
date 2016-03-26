
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