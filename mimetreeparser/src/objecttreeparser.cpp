/*
    objecttreeparser.cpp

    This file is part of KMail, the KDE mail client.
    Copyright (c) 2003      Marc Mutz <mutz@kde.org>
    Copyright (C) 2002-2004 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    Copyright (c) 2009 Andras Mantia <andras@kdab.net>
    Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

    KMail is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    KMail is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

// MessageViewer includes

#include "objecttreeparser.h"

#include "bodypartformatterfactory.h"
#include "nodehelper.h"
#include "messagepart.h"
#include "partnodebodypart.h"

#include "mimetreeparser_debug.h"

#include "bodyformatter/utils.h"
#include "interfaces/bodypartformatter.h"
#include "utils/util.h"

#include <KMime/Headers>
#include <KMime/Message>

// KDE includes

// Qt includes
#include <QByteArray>
#include <QTextCodec>

using namespace MimeTreeParser;

ObjectTreeParser::ObjectTreeParser(const ObjectTreeParser *topLevelParser)
    : mSource(topLevelParser->mSource)
    , mNodeHelper(topLevelParser->mNodeHelper)
    , mTopLevelContent(topLevelParser->mTopLevelContent)
    , mHasPendingAsyncJobs(false)
    , mAllowAsync(topLevelParser->mAllowAsync)
{
    init();
}

ObjectTreeParser::ObjectTreeParser(Interface::ObjectTreeSource *source, MimeTreeParser::NodeHelper *nodeHelper)
    : mSource(source)
    , mNodeHelper(nodeHelper)
    , mTopLevelContent(nullptr)
    , mHasPendingAsyncJobs(false)
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

        mSource->render(mParsedPart, parseOnlySingleNode);
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
        if (!result) {
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
        return MessagePart::Ptr();
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
            if (node->contentType(false) && !node->contentType()->mimeType().isEmpty()) {
                mimeType = node->contentType()->mimeType();
            }
            // unfortunately there's many emails where we can't trust the attachment mimetype
            // so try to see if we can find something better
            if (mimeType == "application/octet-stream") {
                NodeHelper::magicSetType(node);
                mimeType = node->contentType()->mimeType();
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
    if ((inlineSignatureState() != KMMsgNotSigned)
        || (inlineEncryptionState() != KMMsgNotEncrypted)) {
        mNodeHelper->setSignatureState(node, inlineSignatureState());
        mNodeHelper->setEncryptionState(node, inlineEncryptionState());
    }
}

void ObjectTreeParser::extractNodeInfos(KMime::Content *curNode, bool isFirstTextPart)
{
    if (isFirstTextPart) {
        mPlainTextContent += curNode->decodedText();
        mPlainTextContentCharset += NodeHelper::charset(curNode);
    }
}

void ObjectTreeParser::setPlainTextContent(const QString &plainTextContent)
{
    mPlainTextContent = plainTextContent;
}

const QTextCodec *ObjectTreeParser::codecFor(KMime::Content *node) const
{
    Q_ASSERT(node);
    if (mSource->overrideCodec()) {
        return mSource->overrideCodec();
    }
    return mNodeHelper->codec(node);
}

QByteArray ObjectTreeParser::plainTextContentCharset() const
{
    return mPlainTextContentCharset;
}

QByteArray ObjectTreeParser::htmlContentCharset() const
{
    return mHtmlContentCharset;
}

MimeTreeParser::NodeHelper *ObjectTreeParser::nodeHelper() const
{
    return mNodeHelper;
}
