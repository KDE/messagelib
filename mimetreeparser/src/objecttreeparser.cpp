/*
    objecttreeparser.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2002-2004 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>
    SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// MessageViewer includes

#include "objecttreeparser.h"

#include "bodypartformatterfactory.h"
#include "messagepart.h"
#include "nodehelper.h"
#include "partnodebodypart.h"

#include "bodyformatter/utils.h"
#include "interfaces/bodypartformatter.h"
#include "utils/util.h"

#include <KMime/Headers>
#include <KMime/Message>

// Qt includes
#include <QByteArray>

using namespace MimeTreeParser;

ObjectTreeParser::ObjectTreeParser(const ObjectTreeParser *topLevelParser)
    : mSource(topLevelParser->mSource)
    , mNodeHelper(topLevelParser->mNodeHelper)
    , mTopLevelContent(topLevelParser->mTopLevelContent)
    , mAllowAsync(topLevelParser->mAllowAsync)
{
    init();
}

ObjectTreeParser::ObjectTreeParser(Interface::ObjectTreeSource *source, MimeTreeParser::NodeHelper *nodeHelper)
    : mSource(source)
    , mNodeHelper(nodeHelper)
    , mTopLevelContent(nullptr)
    , mAllowAsync(false)
{
    init();
}

void ObjectTreeParser::init()
{
    Q_ASSERT(mSource);

    if (!mNodeHelper) {
        mNodeHelper = new NodeHelper();
        mDeleteNodeHelper = true;
    } else {
        mDeleteNodeHelper = false;
    }
}

ObjectTreeParser::ObjectTreeParser(const ObjectTreeParser &other)
    : mSource(other.mSource)
    , mNodeHelper(other.nodeHelper())
    , mTopLevelContent(other.mTopLevelContent)
    , mHasPendingAsyncJobs(other.hasPendingAsyncJobs())
    , mAllowAsync(other.allowAsync())
    , mDeleteNodeHelper(false)
{
}

ObjectTreeParser::~ObjectTreeParser()
{
    if (mDeleteNodeHelper) {
        delete mNodeHelper;
        mNodeHelper = nullptr;
    }
}

void ObjectTreeParser::setAllowAsync(bool allow)
{
    Q_ASSERT(!mHasPendingAsyncJobs);
    mAllowAsync = allow;
}

bool ObjectTreeParser::allowAsync() const
{
    return mAllowAsync;
}

bool ObjectTreeParser::hasPendingAsyncJobs() const
{
    return mHasPendingAsyncJobs;
}

QString ObjectTreeParser::plainTextContent() const
{
    return mPlainTextContent;
}

QString ObjectTreeParser::htmlContent() const
{
    return mHtmlContent;
}

//-----------------------------------------------------------------------------

void ObjectTreeParser::parseObjectTree(KMime::Content *node, bool parseOnlySingleNode)
{
    mTopLevelContent = node;
    mParsedPart = parseObjectTreeInternal(node, parseOnlySingleNode);

    if (mParsedPart) {
        mParsedPart->fix();
        if (auto mp = toplevelTextNode(mParsedPart)) {
            if (auto _mp = mp.dynamicCast<TextMessagePart>()) {
                extractNodeInfos(_mp->content(), true);
            } else if (auto _mp = mp.dynamicCast<AlternativeMessagePart>()) {
                if (_mp->childParts().contains(Util::MultipartPlain)) {
                    extractNodeInfos(_mp->childParts()[Util::MultipartPlain]->content(), true);
                }
            }
            setPlainTextContent(mp->text());
        }
    }
}

MessagePartPtr ObjectTreeParser::parsedPart() const
{
    return mParsedPart;
}

MessagePartPtr ObjectTreeParser::processType(KMime::Content *node, ProcessResult &processResult, const QByteArray &mimeType)
{
    const auto formatters = mSource->bodyPartFormatterFactory()->formattersForType(QString::fromUtf8(mimeType));
    Q_ASSERT(!formatters.empty());
    for (auto formatter : formatters) {
        PartNodeBodyPart part(this, &processResult, mTopLevelContent, node, mNodeHelper);
        mNodeHelper->setNodeDisplayedEmbedded(node, true);

        const MessagePart::Ptr result = formatter->process(part);
        if (result.isNull()) {
            continue;
        }

        result->setAttachmentContent(node);
        return result;
    }

    Q_UNREACHABLE();
    return {};
}

MessagePart::Ptr ObjectTreeParser::parseObjectTreeInternal(KMime::Content *node, bool onlyOneMimePart)
{
    if (!node) {
        return {};
    }

    // reset pending async jobs state (we'll rediscover pending jobs as we go)
    mHasPendingAsyncJobs = false;

    mNodeHelper->clearOverrideHeaders();

    // reset "processed" flags for...
    if (onlyOneMimePart) {
        // ... this node and all descendants
        mNodeHelper->setNodeUnprocessed(node, false);
        if (!node->contents().isEmpty()) {
            mNodeHelper->setNodeUnprocessed(node, true);
        }
    } else if (!node->parent()) {
        // ...this node and all it's siblings and descendants
        mNodeHelper->setNodeUnprocessed(node, true);
    }

    const bool isRoot = node->isTopLevel();
    auto parsedPart = MessagePart::Ptr(new MessagePartList(this));
    parsedPart->setIsRoot(isRoot);
    KMime::Content *parent = node->parent();
    auto contents = parent ? parent->contents() : KMime::Content::List();
    if (contents.isEmpty()) {
        contents.append(node);
    }
    int i = contents.indexOf(node);
    if (i < 0) {
        return parsedPart;
    } else {
        for (; i < contents.size(); ++i) {
            node = contents.at(i);
            if (mNodeHelper->nodeProcessed(node)) {
                continue;
            }

            ProcessResult processResult(mNodeHelper);

            QByteArray mimeType("text/plain");
            if (auto ct = node->contentType(false)) {
                mimeType = ct->mimeType();
            }
            // unfortunately there's many emails where we can't trust the attachment mimetype
            // so try to see if we can find something better
            if (mimeType == "application/octet-stream") {
                NodeHelper::magicSetType(node);
                mimeType = node->contentType(false)->mimeType();
            }

            const auto mp = processType(node, processResult, mimeType);
            Q_ASSERT(mp);
            parsedPart->appendSubPart(mp);
            mNodeHelper->setNodeProcessed(node, false);

            // adjust signed/encrypted flags if inline PGP was found
            processResult.adjustCryptoStatesOfNode(node);

            if (onlyOneMimePart) {
                break;
            }
        }
    }
    return parsedPart;
}

KMMsgSignatureState ProcessResult::inlineSignatureState() const
{
    return mInlineSignatureState;
}

void ProcessResult::setInlineSignatureState(KMMsgSignatureState state)
{
    mInlineSignatureState = state;
}

KMMsgEncryptionState ProcessResult::inlineEncryptionState() const
{
    return mInlineEncryptionState;
}

void ProcessResult::setInlineEncryptionState(KMMsgEncryptionState state)
{
    mInlineEncryptionState = state;
}

bool ProcessResult::neverDisplayInline() const
{
    return mNeverDisplayInline;
}

void ProcessResult::setNeverDisplayInline(bool display)
{
    mNeverDisplayInline = display;
}

void ProcessResult::adjustCryptoStatesOfNode(const KMime::Content *node) const
{
    if ((inlineSignatureState() != KMMsgNotSigned) || (inlineEncryptionState() != KMMsgNotEncrypted)) {
        mNodeHelper->setSignatureState(node, inlineSignatureState());
        mNodeHelper->setEncryptionState(node, inlineEncryptionState());
    }
}

void ObjectTreeParser::extractNodeInfos(KMime::Content *curNode, bool isFirstTextPart)
{
    if (isFirstTextPart) {
        mPlainTextContent += curNode->decodedText();
    }
}

void ObjectTreeParser::setPlainTextContent(const QString &plainTextContent)
{
    mPlainTextContent = plainTextContent;
}

QByteArray ObjectTreeParser::codecNameFor(KMime::Content *node) const
{
    Q_ASSERT(node);
    if (!mSource->overrideCodecName().isEmpty()) {
        return mSource->overrideCodecName();
    }
    return mNodeHelper->codecName(node);
}

MimeTreeParser::NodeHelper *ObjectTreeParser::nodeHelper() const
{
    return mNodeHelper;
}
