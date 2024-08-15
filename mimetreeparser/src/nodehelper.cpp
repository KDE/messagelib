/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "nodehelper.h"
#include "interfaces/bodypart.h"
#include "messagepart.h"
#include "mimetreeparser_debug.h"
#include "partmetadata.h"
#include "temporaryfile/attachmenttemporaryfilesdirs.h"

#include <KMime/Content>

#include <KCharsets>
#include <KLocalizedString>
#include <QTemporaryFile>

#include <QDir>
#include <QRegularExpressionMatch>
#include <QStringDecoder>
#include <QUrl>

#include <QFileDevice>
#include <QMimeDatabase>
#include <QMimeType>
#include <algorithm>
#include <sstream>
#include <string>

namespace MimeTreeParser
{
NodeHelper::NodeHelper()
    : mAttachmentFilesDir(new AttachmentTemporaryFilesDirs())
{
    mListAttachmentTemporaryDirs.append(mAttachmentFilesDir);
    // TODO(Andras) add methods to modify these prefixes
}

NodeHelper::~NodeHelper()
{
    for (auto att : mListAttachmentTemporaryDirs) {
        if (att) {
            att->forceCleanTempFiles();
            delete att;
        }
    }
    clear();
}

void NodeHelper::setNodeProcessed(KMime::Content *node, bool recurse)
{
    if (!node) {
        return;
    }
    mProcessedNodes.append(node);
    qCDebug(MIMETREEPARSER_LOG) << "Node processed: " << node->index().toString() << node->contentType()->as7BitString();
    //<< " decodedContent" << node->decodedContent();
    if (recurse) {
        const auto contents = node->contents();
        for (KMime::Content *c : contents) {
            setNodeProcessed(c, true);
        }
    }
}

void NodeHelper::setNodeUnprocessed(KMime::Content *node, bool recurse)
{
    if (!node) {
        return;
    }
    mProcessedNodes.removeAll(node);

    // avoid double addition of extra nodes, eg. encrypted attachments
    const QMap<KMime::Content *, QList<KMime::Content *>>::iterator it = mExtraContents.find(node);
    if (it != mExtraContents.end()) {
        const auto contents = it.value();
        for (KMime::Content *c : contents) {
            KMime::Content *p = c->parent();
            if (p) {
                p->takeContent(c);
            }
        }
        qDeleteAll(it.value());
        qCDebug(MIMETREEPARSER_LOG) << "mExtraContents deleted for" << it.key();
        mExtraContents.erase(it);
    }

    qCDebug(MIMETREEPARSER_LOG) << "Node UNprocessed: " << node;
    if (recurse) {
        const auto contents = node->contents();
        for (KMime::Content *c : contents) {
            setNodeUnprocessed(c, true);
        }
    }
}

bool NodeHelper::nodeProcessed(KMime::Content *node) const
{
    if (!node) {
        return true;
    }
    return mProcessedNodes.contains(node);
}

static void clearBodyPartMemento(QMap<QByteArray, Interface::BodyPartMemento *> &bodyPartMementoMap)
{
    for (QMap<QByteArray, Interface::BodyPartMemento *>::iterator it = bodyPartMementoMap.begin(), end = bodyPartMementoMap.end(); it != end; ++it) {
        Interface::BodyPartMemento *memento = it.value();
        memento->detach();
        delete memento;
    }
    bodyPartMementoMap.clear();
}

void NodeHelper::clear()
{
    mProcessedNodes.clear();
    mEncryptionState.clear();
    mSignatureState.clear();
    mOverrideCodecs.clear();
    std::for_each(mBodyPartMementoMap.begin(), mBodyPartMementoMap.end(), &clearBodyPartMemento);
    mBodyPartMementoMap.clear();
    QMap<KMime::Content *, QList<KMime::Content *>>::ConstIterator end(mExtraContents.constEnd());

    for (QMap<KMime::Content *, QList<KMime::Content *>>::ConstIterator it = mExtraContents.constBegin(); it != end; ++it) {
        const auto contents = it.value();
        for (KMime::Content *c : contents) {
            KMime::Content *p = c->parent();
            if (p) {
                p->takeContent(c);
            }
        }
        qDeleteAll(it.value());
        qCDebug(MIMETREEPARSER_LOG) << "mExtraContents deleted for" << it.key();
    }
    mExtraContents.clear();
    mDisplayEmbeddedNodes.clear();
    mDisplayHiddenNodes.clear();
}

void NodeHelper::setEncryptionState(const KMime::Content *node, const KMMsgEncryptionState state)
{
    mEncryptionState[node] = state;
}

KMMsgEncryptionState NodeHelper::encryptionState(const KMime::Content *node) const
{
    return mEncryptionState.value(node, KMMsgNotEncrypted);
}

void NodeHelper::setSignatureState(const KMime::Content *node, const KMMsgSignatureState state)
{
    mSignatureState[node] = state;
}

KMMsgSignatureState NodeHelper::signatureState(const KMime::Content *node) const
{
    return mSignatureState.value(node, KMMsgNotSigned);
}

PartMetaData NodeHelper::partMetaData(KMime::Content *node)
{
    return mPartMetaDatas.value(node, PartMetaData());
}

void NodeHelper::setPartMetaData(KMime::Content *node, const PartMetaData &metaData)
{
    mPartMetaDatas.insert(node, metaData);
}

QString NodeHelper::writeFileToTempFile(KMime::Content *node, const QString &filename)
{
    QString fname = createTempDir(persistentIndex(node));
    if (fname.isEmpty()) {
        return {};
    }
    fname += QLatin1Char('/') + filename;
    QFile f(fname);
    if (!f.open(QIODevice::WriteOnly)) {
        qCWarning(MIMETREEPARSER_LOG) << "Failed to write note to file:" << f.errorString();
        mAttachmentFilesDir->addTempFile(fname);
        return {};
    }
    f.write(QByteArray());
    mAttachmentFilesDir->addTempFile(fname);
    // make file read-only so that nobody gets the impression that he might
    // edit attached files (cf. bug #52813)
    f.setPermissions(QFileDevice::ReadUser);
    f.close();

    return fname;
}

QString NodeHelper::writeNodeToTempFile(KMime::Content *node)
{
    // If the message part is already written to a file, no point in doing it again.
    // This function is called twice actually, once from the rendering of the attachment
    // in the body and once for the header.
    const QUrl existingFileName = tempFileUrlFromNode(node);
    if (!existingFileName.isEmpty()) {
        return existingFileName.toLocalFile();
    }

    QString fname = createTempDir(persistentIndex(node));
    if (fname.isEmpty()) {
        return {};
    }

    QString fileName = NodeHelper::fileName(node);
    // strip off a leading path
    int slashPos = fileName.lastIndexOf(QLatin1Char('/'));
    if (-1 != slashPos) {
        fileName = fileName.mid(slashPos + 1);
    }
    if (fileName.isEmpty()) {
        fileName = QStringLiteral("unnamed");
    }
    fname += QLatin1Char('/') + fileName;

    qCDebug(MIMETREEPARSER_LOG) << "Create temp file: " << fname;
    QByteArray data = node->decodedContent();
    if (node->contentType()->isText() && !data.isEmpty()) {
        // convert CRLF to LF before writing text attachments to disk
        data = KMime::CRLFtoLF(data);
    }
    QFile f(fname);
    if (!f.open(QIODevice::WriteOnly)) {
        qCWarning(MIMETREEPARSER_LOG) << "Failed to write note to file:" << f.errorString();
        mAttachmentFilesDir->addTempFile(fname);
        return {};
    }
    f.write(data);
    mAttachmentFilesDir->addTempFile(fname);
    // make file read-only so that nobody gets the impression that he might
    // edit attached files (cf. bug #52813)
    f.setPermissions(QFileDevice::ReadUser);
    f.close();

    return fname;
}

QUrl NodeHelper::tempFileUrlFromNode(const KMime::Content *node)
{
    if (!node) {
        return {};
    }

    const QString index = persistentIndex(node);

    const QStringList temporaryFiles = mAttachmentFilesDir->temporaryFiles();
    for (const QString &path : temporaryFiles) {
        const int right = path.lastIndexOf(QLatin1Char('/'));
        int left = path.lastIndexOf(QLatin1StringView(".index."), right);
        if (left != -1) {
            left += 7;
        }
        const QStringView strView(path);
        const QStringView storedIndex = strView.sliced(left, right - left);
        if (left != -1 && storedIndex == index) {
            return QUrl::fromLocalFile(path);
        }
    }
    return {};
}

QString NodeHelper::createTempDir(const QString &param)
{
    auto tempFile = new QTemporaryFile(QDir::tempPath() + QLatin1StringView("/messageviewer_XXXXXX") + QLatin1StringView(".index.") + param);
    tempFile->open();
    const QString fname = tempFile->fileName();
    delete tempFile;

    QFile fFile(fname);
    if (!(fFile.permissions() & QFileDevice::WriteUser)) {
        // Not there or not writable
        if (!QDir().mkpath(fname) || !fFile.setPermissions(QFileDevice::WriteUser | QFileDevice::ReadUser | QFileDevice::ExeUser)) {
            mAttachmentFilesDir->addTempDir(fname);
            return {}; // failed create
        }
    }

    Q_ASSERT(!fname.isNull());

    mAttachmentFilesDir->addTempDir(fname);
    return fname;
}

void NodeHelper::forceCleanTempFiles()
{
    mAttachmentFilesDir->forceCleanTempFiles();
    delete mAttachmentFilesDir;
    mAttachmentFilesDir = nullptr;
}

void NodeHelper::removeTempFiles()
{
    // Don't delete as it will be deleted in class
    if (mAttachmentFilesDir) {
        mAttachmentFilesDir->removeTempFiles();
    }
    mAttachmentFilesDir = new AttachmentTemporaryFilesDirs();
    mListAttachmentTemporaryDirs.append(mAttachmentFilesDir);
}

void NodeHelper::addTempFile(const QString &file)
{
    mAttachmentFilesDir->addTempFile(file);
}

bool NodeHelper::isInEncapsulatedMessage(KMime::Content *node)
{
    const KMime::Content *const topLevel = node->topLevel();
    const KMime::Content *cur = node;
    while (cur && cur != topLevel) {
        const bool parentIsMessage = cur->parent() && cur->parent()->contentType() && cur->parent()->contentType()->mimeType().toLower() == "message/rfc822";
        if (parentIsMessage && cur->parent() != topLevel) {
            return true;
        }
        cur = cur->parent();
    }
    return false;
}

QByteArray NodeHelper::charset(const KMime::Content *node)
{
    if (const auto ct = node->contentType(); ct) {
        return ct->charset();
    }
    return QByteArrayLiteral("utf-8");
}

KMMsgEncryptionState NodeHelper::overallEncryptionState(KMime::Content *node) const
{
    KMMsgEncryptionState myState = KMMsgEncryptionStateUnknown;
    if (!node) {
        return myState;
    }

    KMime::Content *parent = node->parent();
    auto contents = parent ? parent->contents() : KMime::Content::List();
    if (contents.isEmpty()) {
        contents.append(node);
    }
    int i = contents.indexOf(const_cast<KMime::Content *>(node));
    if (i < 0) {
        return myState;
    }
    for (; i < contents.size(); ++i) {
        auto next = contents.at(i);
        KMMsgEncryptionState otherState = encryptionState(next);

        // NOTE: children are tested ONLY when parent is not encrypted
        if (otherState == KMMsgNotEncrypted && !next->contents().isEmpty()) {
            otherState = overallEncryptionState(next->contents().at(0));
        }

        if (otherState == KMMsgNotEncrypted && !extraContents(next).isEmpty()) {
            otherState = overallEncryptionState(extraContents(next).at(0));
        }

        if (next == node) {
            myState = otherState;
        }

        switch (otherState) {
        case KMMsgEncryptionStateUnknown:
            break;
        case KMMsgNotEncrypted:
            if (myState == KMMsgFullyEncrypted) {
                myState = KMMsgPartiallyEncrypted;
            } else if (myState != KMMsgPartiallyEncrypted) {
                myState = KMMsgNotEncrypted;
            }
            break;
        case KMMsgPartiallyEncrypted:
            myState = KMMsgPartiallyEncrypted;
            break;
        case KMMsgFullyEncrypted:
            if (myState != KMMsgFullyEncrypted) {
                myState = KMMsgPartiallyEncrypted;
            }
            break;
        case KMMsgEncryptionProblematic:
            break;
        }
    }

    qCDebug(MIMETREEPARSER_LOG) << "\n\n  KMMsgEncryptionState:" << myState;

    return myState;
}

KMMsgSignatureState NodeHelper::overallSignatureState(KMime::Content *node) const
{
    KMMsgSignatureState myState = KMMsgSignatureStateUnknown;
    if (!node) {
        return myState;
    }

    KMime::Content *parent = node->parent();
    auto contents = parent ? parent->contents() : KMime::Content::List();
    if (contents.isEmpty()) {
        contents.append(node);
    }
    int i = contents.indexOf(const_cast<KMime::Content *>(node));
    if (i < 0) { // Be safe
        return myState;
    }
    for (; i < contents.size(); ++i) {
        auto next = contents.at(i);
        KMMsgSignatureState otherState = signatureState(next);

        // NOTE: children are tested ONLY when parent is not encrypted
        if (otherState == KMMsgNotSigned && !next->contents().isEmpty()) {
            otherState = overallSignatureState(next->contents().at(0));
        }

        if (otherState == KMMsgNotSigned && !extraContents(next).isEmpty()) {
            otherState = overallSignatureState(extraContents(next).at(0));
        }

        if (next == node) {
            myState = otherState;
        }

        switch (otherState) {
        case KMMsgSignatureStateUnknown:
            break;
        case KMMsgNotSigned:
            if (myState == KMMsgFullySigned) {
                myState = KMMsgPartiallySigned;
            } else if (myState != KMMsgPartiallySigned) {
                myState = KMMsgNotSigned;
            }
            break;
        case KMMsgPartiallySigned:
            myState = KMMsgPartiallySigned;
            break;
        case KMMsgFullySigned:
            if (myState != KMMsgFullySigned) {
                myState = KMMsgPartiallySigned;
            }
            break;
        case KMMsgSignatureProblematic:
            break;
        }
    }

    qCDebug(MIMETREEPARSER_LOG) << "\n\n  KMMsgSignatureState:" << myState;

    return myState;
}

void NodeHelper::magicSetType(KMime::Content *node, bool aAutoDecode)
{
    const QByteArray body = aAutoDecode ? node->decodedContent() : node->body();
    QMimeDatabase db;
    const QMimeType mime = db.mimeTypeForData(body);

    const QString mimetype = mime.name();
    node->contentType()->setMimeType(mimetype.toLatin1());
}

bool NodeHelper::hasMailHeader(const char *header, const KMime::Content *message) const
{
    if (mHeaderOverwrite.contains(message)) {
        const auto parts = mHeaderOverwrite.value(message);
        for (const auto &messagePart : parts) {
            if (messagePart->hasHeader(header)) {
                return true;
            }
        }
    }
    return message->hasHeader(header);
}

QList<MessagePart::Ptr> NodeHelper::messagePartsOfMailHeader(const char *header, const KMime::Content *message) const
{
    QList<MessagePart::Ptr> ret;
    if (mHeaderOverwrite.contains(message)) {
        const auto parts = mHeaderOverwrite.value(message);
        for (const auto &messagePart : parts) {
            if (messagePart->hasHeader(header)) {
                ret << messagePart;
            }
        }
    }
    return ret;
}

QList<KMime::Headers::Base *> NodeHelper::headers(const char *header, const KMime::Content *message)
{
    const auto mp = messagePartsOfMailHeader(header, message);
    if (mp.size() > 0) {
        return mp.value(0)->headers(header);
    }

    return message->headersByType(header);
}

KMime::Headers::Base const *NodeHelper::mailHeaderAsBase(const char *header, const KMime::Content *message) const
{
    if (mHeaderOverwrite.contains(message)) {
        const auto parts = mHeaderOverwrite.value(message);
        for (const auto &messagePart : parts) {
            if (messagePart->hasHeader(header)) {
                return messagePart->header(header); // Found.
            }
        }
    }
    return message->headerByType(header);
}

QSharedPointer<KMime::Headers::Generics::AddressList> NodeHelper::mailHeaderAsAddressList(const char *header, const KMime::Content *message) const
{
    const auto hrd = mailHeaderAsBase(header, message);
    if (!hrd) {
        return nullptr;
    }
    QSharedPointer<KMime::Headers::Generics::AddressList> addressList(new KMime::Headers::Generics::AddressList());
    addressList->from7BitString(hrd->as7BitString(false));
    return addressList;
}

void NodeHelper::clearOverrideHeaders()
{
    mHeaderOverwrite.clear();
}

void NodeHelper::registerOverrideHeader(KMime::Content *message, MessagePart::Ptr part)
{
    if (!mHeaderOverwrite.contains(message)) {
        mHeaderOverwrite[message] = QList<MessagePart::Ptr>();
    }
    mHeaderOverwrite[message].append(part);
}

QDateTime NodeHelper::dateHeader(KMime::Content *message) const
{
    const auto dateHeader = mailHeaderAsBase("date", message);
    if (dateHeader != nullptr) {
        return static_cast<const KMime::Headers::Date *>(dateHeader)->dateTime();
    }
    return {};
}

void NodeHelper::setOverrideCodec(KMime::Content *node, const QByteArray &codec)
{
    if (!node) {
        return;
    }

    mOverrideCodecs[node] = codec;
}

QByteArray NodeHelper::codecName(KMime::Content *node) const
{
    if (!node || !node->contentType()) {
        return "utf-8";
    }

    auto c = mOverrideCodecs.value(node);
    if (c.isEmpty()) {
        // no override-codec set for this message, try the CT charset parameter:
        c = node->contentType()->charset();

        // utf-8 is a superset of us-ascii, so we don't loose anything, if we it insead
        // utf-8 is nowadays that widely, that it is a good guess to use it to fix issus with broken clients.
        if (c.toLower() == "us-ascii") {
            c = "utf-8";
        }
        if (QStringDecoder codec(c.constData()); !codec.isValid()) {
            c = "utf-8";
        }
    }
    if (c.isEmpty()) {
        // no charset means us-ascii (RFC 2045), so using local encoding should
        // be okay
        c = "UTF-8";
    }
    return c;
}

QString NodeHelper::fileName(const KMime::Content *node)
{
    QString name = const_cast<KMime::Content *>(node)->contentDisposition()->filename();
    if (name.isEmpty()) {
        name = const_cast<KMime::Content *>(node)->contentType()->name();
    }

    name = name.trimmed();
    return name;
}

// FIXME(Andras) review it (by Marc?) to see if I got it right. This is supposed to be the partNode::internalBodyPartMemento replacement
Interface::BodyPartMemento *NodeHelper::bodyPartMemento(KMime::Content *node, const QByteArray &which) const
{
    const QMap<QString, QMap<QByteArray, Interface::BodyPartMemento *>>::const_iterator nit = mBodyPartMementoMap.find(persistentIndex(node));
    if (nit == mBodyPartMementoMap.end()) {
        return nullptr;
    }
    const QMap<QByteArray, Interface::BodyPartMemento *>::const_iterator it = nit->find(which.toLower());
    return it != nit->end() ? it.value() : nullptr;
}

// FIXME(Andras) review it (by Marc?) to see if I got it right. This is supposed to be the partNode::internalSetBodyPartMemento replacement
void NodeHelper::setBodyPartMemento(KMime::Content *node, const QByteArray &which, Interface::BodyPartMemento *memento)
{
    QMap<QByteArray, Interface::BodyPartMemento *> &mementos = mBodyPartMementoMap[persistentIndex(node)];

    const QByteArray whichLower = which.toLower();
    const QMap<QByteArray, Interface::BodyPartMemento *>::iterator it = mementos.lowerBound(whichLower);

    if (it != mementos.end() && it.key() == whichLower) {
        delete it.value();
        if (memento) {
            it.value() = memento;
        } else {
            mementos.erase(it);
        }
    } else {
        mementos.insert(whichLower, memento);
    }
}

bool NodeHelper::isNodeDisplayedEmbedded(KMime::Content *node) const
{
    qCDebug(MIMETREEPARSER_LOG) << "IS NODE: " << mDisplayEmbeddedNodes.contains(node);
    return mDisplayEmbeddedNodes.contains(node);
}

void NodeHelper::setNodeDisplayedEmbedded(KMime::Content *node, bool displayedEmbedded)
{
    qCDebug(MIMETREEPARSER_LOG) << "SET NODE: " << node << displayedEmbedded;
    if (displayedEmbedded) {
        mDisplayEmbeddedNodes.insert(node);
    } else {
        mDisplayEmbeddedNodes.remove(node);
    }
}

bool NodeHelper::isNodeDisplayedHidden(KMime::Content *node) const
{
    return mDisplayHiddenNodes.contains(node);
}

void NodeHelper::setNodeDisplayedHidden(KMime::Content *node, bool displayedHidden)
{
    if (displayedHidden) {
        mDisplayHiddenNodes.insert(node);
    } else {
        mDisplayEmbeddedNodes.remove(node);
    }
}

/*!
  Creates a persistent index string that bridges the gap between the
  permanent nodes and the temporary ones.

  Used internally for robust indexing.
*/
QString NodeHelper::persistentIndex(const KMime::Content *node) const
{
    if (!node) {
        return {};
    }

    QString indexStr = node->index().toString();
    if (indexStr.isEmpty()) {
        QMapIterator<KMime::Message::Content *, QList<KMime::Content *>> it(mExtraContents);
        while (it.hasNext()) {
            it.next();
            const auto &extraNodes = it.value();
            for (int i = 0; i < extraNodes.size(); ++i) {
                if (extraNodes[i] == node) {
                    indexStr = QStringLiteral("e%1").arg(i);
                    const QString parentIndex = persistentIndex(it.key());
                    if (!parentIndex.isEmpty()) {
                        indexStr = QStringLiteral("%1:%2").arg(parentIndex, indexStr);
                    }
                    return indexStr;
                }
            }
        }
    } else {
        const KMime::Content *const topLevel = node->topLevel();
        // if the node is an extra node, prepend the index of the extra node to the url
        QMapIterator<KMime::Message::Content *, QList<KMime::Content *>> it(mExtraContents);
        while (it.hasNext()) {
            it.next();
            const QList<KMime::Content *> &extraNodes = extraContents(it.key());
            for (int i = 0; i < extraNodes.size(); ++i) {
                KMime::Content *const extraNode = extraNodes[i];
                if (topLevel == extraNode) {
                    indexStr.prepend(QStringLiteral("e%1:").arg(i));
                    const QString parentIndex = persistentIndex(it.key());
                    if (!parentIndex.isEmpty()) {
                        indexStr = QStringLiteral("%1:%2").arg(parentIndex, indexStr);
                    }
                    return indexStr;
                }
            }
        }
    }

    return indexStr;
}

KMime::Content *NodeHelper::contentFromIndex(KMime::Content *node, const QString &persistentIndex) const
{
    if (!node) {
        return nullptr;
    }
    KMime::Content *c = node->topLevel();
    if (c) {
        const QStringList pathParts = persistentIndex.split(QLatin1Char(':'), Qt::SkipEmptyParts);
        const int pathPartsSize(pathParts.size());
        for (int i = 0; i < pathPartsSize; ++i) {
            const QString &path = pathParts[i];
            if (path.startsWith(QLatin1Char('e'))) {
                const QList<KMime::Content *> &extraParts = mExtraContents.value(c);
                const int idx = QStringView(path).mid(1).toInt();
                c = (idx < extraParts.size()) ? extraParts[idx] : nullptr;
            } else {
                c = c->content(KMime::ContentIndex(path));
            }
            if (!c) {
                break;
            }
        }
    }
    return c;
}

QString NodeHelper::asHREF(const KMime::Content *node, const QString &place) const
{
    return QStringLiteral("attachment:%1?place=%2").arg(persistentIndex(node), place);
}

QString NodeHelper::asHREF(const KMime::Content *node) const
{
    return QStringLiteral("attachment:%1").arg(persistentIndex(node));
}

KMime::Content *NodeHelper::fromHREF(const KMime::Message::Ptr &mMessage, const QUrl &url) const
{
    if (url.isEmpty()) {
        return mMessage.data();
    }

    if (!url.isLocalFile()) {
        return contentFromIndex(mMessage.data(), url.adjusted(QUrl::StripTrailingSlash).path());
    } else {
        const QString path = url.toLocalFile();
        const QString extractedPath = extractAttachmentIndex(path);
        if (!extractedPath.isEmpty()) {
            return contentFromIndex(mMessage.data(), extractedPath);
        }
        return mMessage.data();
    }
}

QString NodeHelper::extractAttachmentIndex(const QString &path) const
{
    // extract from /<path>/qttestn28554.index.2.3:0:2/unnamed -> "2.3:0:2"
    // start of the index is something that is not a number followed by a dot: \D.
    // index is only made of numbers,"." and ":": ([0-9.:]+)
    // index is the last part of the folder name: /
    static const QRegularExpression re(QStringLiteral("\\D\\.([e0-9.:]+)/"));
    QRegularExpressionMatch rmatch;
    path.lastIndexOf(re, -1, &rmatch);
    if (rmatch.hasMatch()) {
        return rmatch.captured(1);
    }
    return {};
}

QString NodeHelper::fixEncoding(const QString &encoding)
{
    QString returnEncoding = encoding;
    // According to https://www.iana.org/assignments/character-sets, uppercase is
    // preferred in MIME headers
    const QString returnEncodingToUpper = returnEncoding.toUpper();
    if (returnEncodingToUpper.contains(QLatin1StringView("ISO "))) {
        returnEncoding = returnEncodingToUpper;
        returnEncoding.replace(QLatin1StringView("ISO "), QStringLiteral("ISO-"));
    }
    return returnEncoding;
}

//-----------------------------------------------------------------------------
QString NodeHelper::encodingForName(const QString &descriptiveName)
{
    const QString encoding = KCharsets::charsets()->encodingForName(descriptiveName);
    return NodeHelper::fixEncoding(encoding);
}

QStringList NodeHelper::supportedEncodings()
{
    QStringList encodingNames = KCharsets::charsets()->availableEncodingNames();
    QStringList encodings;
    QMap<QString, bool> mimeNames;
    QStringList::ConstIterator constEnd(encodingNames.constEnd());
    for (QStringList::ConstIterator it = encodingNames.constBegin(); it != constEnd; ++it) {
        QStringDecoder codec((*it).toLatin1().constData());
        const QString mimeName = (codec.isValid()) ? QString::fromLatin1(codec.name()).toLower() : (*it);
        if (!mimeNames.contains(mimeName)) {
            encodings.append(KCharsets::charsets()->descriptionForEncoding(*it));
            mimeNames.insert(mimeName, true);
        }
    }
    encodings.sort();
    return encodings;
}

QString NodeHelper::fromAsString(KMime::Content *node) const
{
    if (auto topLevel = dynamic_cast<KMime::Message *>(node->topLevel())) {
        return topLevel->from()->asUnicodeString();
    } else {
        auto realNode = std::find_if(mExtraContents.cbegin(), mExtraContents.cend(), [node](const QList<KMime::Content *> &nodes) {
            return nodes.contains(node);
        });
        if (realNode != mExtraContents.cend()) {
            return fromAsString(realNode.key());
        }
    }

    return {};
}

void NodeHelper::attachExtraContent(KMime::Content *topLevelNode, KMime::Content *content)
{
    qCDebug(MIMETREEPARSER_LOG) << "mExtraContents added for" << topLevelNode << " extra content: " << content;
    mExtraContents[topLevelNode].append(content);
}

void NodeHelper::cleanExtraContent(KMime::Content *topLevelNode)
{
    qCDebug(MIMETREEPARSER_LOG) << "remove all extraContents for" << topLevelNode;
    mExtraContents[topLevelNode].clear();
}

QList<KMime::Content *> NodeHelper::extraContents(KMime::Content *topLevelnode) const
{
    return mExtraContents.value(topLevelnode);
}

void NodeHelper::mergeExtraNodes(KMime::Content *node)
{
    if (!node) {
        return;
    }

    const QList<KMime::Content *> extraNodes = extraContents(node);
    for (KMime::Content *extra : extraNodes) {
        if (node->bodyIsMessage()) {
            qCWarning(MIMETREEPARSER_LOG) << "Asked to attach extra content to a kmime::message, this does not make sense. Attaching to:" << node
                                          << node->encodedContent() << "\n====== with =======\n"
                                          << extra << extra->encodedContent();
            continue;
        }
        auto c = new KMime::Content(node);
        c->setContent(extra->encodedContent());
        c->parse();
        node->appendContent(c);
    }

    const auto contents{node->contents()};
    for (KMime::Content *child : contents) {
        mergeExtraNodes(child);
    }
}

void NodeHelper::cleanFromExtraNodes(KMime::Content *node)
{
    if (!node) {
        return;
    }
    const QList<KMime::Content *> extraNodes = extraContents(node);
    for (KMime::Content *extra : extraNodes) {
        QByteArray s = extra->encodedContent();
        const auto children = node->contents();
        for (KMime::Content *c : children) {
            if (c->encodedContent() == s) {
                node->takeContent(c);
            }
        }
    }
    const auto contents{node->contents()};
    for (KMime::Content *child : contents) {
        cleanFromExtraNodes(child);
    }
}

KMime::Message *NodeHelper::messageWithExtraContent(KMime::Content *topLevelNode)
{
    /*The merge is done in several steps:
      1) merge the extra nodes into topLevelNode
      2) copy the modified (merged) node tree into a new node tree
      3) restore the original node tree in topLevelNode by removing the extra nodes from it

      The reason is that extra nodes are assigned by pointer value to the nodes in the original tree.
    */
    if (!topLevelNode) {
        return nullptr;
    }

    mergeExtraNodes(topLevelNode);

    auto m = new KMime::Message;
    m->setContent(topLevelNode->encodedContent());
    m->parse();

    cleanFromExtraNodes(topLevelNode);
    //   qCDebug(MIMETREEPARSER_LOG) << "MESSAGE WITH EXTRA: " << m->encodedContent();
    //   qCDebug(MIMETREEPARSER_LOG) << "MESSAGE WITHOUT EXTRA: " << topLevelNode->encodedContent();

    return m;
}

KMime::Content *NodeHelper::decryptedNodeForContent(KMime::Content *content) const
{
    const QList<KMime::Content *> xc = extraContents(content);
    if (!xc.empty()) {
        if (xc.size() == 1) {
            return xc.front();
        } else {
            qCWarning(MIMETREEPARSER_LOG) << "WTF, encrypted node has multiple extra contents?";
        }
    }
    return nullptr;
}

bool NodeHelper::unencryptedMessage_helper(KMime::Content *node, QByteArray &resultingData, bool addHeaders, int recursionLevel)
{
    bool returnValue = false;
    if (node) {
        KMime::Content *curNode = node;
        KMime::Content *decryptedNode = nullptr;
        const QByteArray type = node->contentType(false) ? QByteArray(node->contentType(false)->mediaType()).toLower() : "text";
        const QByteArray subType = node->contentType(false) ? node->contentType(false)->subType().toLower() : "plain";
        const bool isMultipart = node->contentType(false) && node->contentType(false)->isMultipart();
        bool isSignature = false;

        qCDebug(MIMETREEPARSER_LOG) << "(" << recursionLevel << ") Looking at" << type << "/" << subType;

        if (isMultipart) {
            if (subType == "signed") {
                isSignature = true;
            } else if (subType == "encrypted") {
                decryptedNode = decryptedNodeForContent(curNode);
            }
        } else if (type == "application") {
            if (subType == "octet-stream") {
                decryptedNode = decryptedNodeForContent(curNode);
            } else if (subType == "pkcs7-signature") {
                isSignature = true;
            } else if (subType == "pkcs7-mime") {
                // note: subtype pkcs7-mime can also be signed
                //       and we do NOT want to remove the signature!
                if (encryptionState(curNode) != KMMsgNotEncrypted) {
                    decryptedNode = decryptedNodeForContent(curNode);
                }
            }
        }

        if (decryptedNode) {
            qCDebug(MIMETREEPARSER_LOG) << "Current node has an associated decrypted node, adding a modified header "
                                           "and then processing the children.";

            Q_ASSERT(addHeaders);
            KMime::Content headers;
            headers.setHead(curNode->head());
            headers.parse();
            if (auto ct = decryptedNode->contentType(false)) {
                headers.contentType()->from7BitString(ct->as7BitString(false));
            } else {
                headers.removeHeader<KMime::Headers::ContentType>();
            }
            if (auto ct = decryptedNode->contentTransferEncoding(false)) {
                headers.contentTransferEncoding()->from7BitString(ct->as7BitString(false));
            } else {
                headers.removeHeader<KMime::Headers::ContentTransferEncoding>();
            }
            if (auto cd = decryptedNode->contentDisposition(false)) {
                headers.contentDisposition()->from7BitString(cd->as7BitString(false));
            } else {
                headers.removeHeader<KMime::Headers::ContentDisposition>();
            }
            if (auto cd = decryptedNode->contentDescription(false)) {
                headers.contentDescription()->from7BitString(cd->as7BitString(false));
            } else {
                headers.removeHeader<KMime::Headers::ContentDescription>();
            }
            headers.assemble();

            resultingData += headers.head() + '\n';
            unencryptedMessage_helper(decryptedNode, resultingData, false, recursionLevel + 1);

            returnValue = true;
        } else if (isSignature) {
            qCDebug(MIMETREEPARSER_LOG) << "Current node is a signature, adding it as-is.";
            // We can't change the nodes under the signature, as that would invalidate it. Add the signature
            // and its child as-is
            if (addHeaders) {
                resultingData += curNode->head() + '\n';
            }
            resultingData += curNode->encodedBody();
            returnValue = false;
        } else if (isMultipart) {
            qCDebug(MIMETREEPARSER_LOG) << "Current node is a multipart node, adding its header and then processing all children.";
            // Normal multipart node, add the header and all of its children
            bool somethingChanged = false;
            if (addHeaders) {
                resultingData += curNode->head() + '\n';
            }
            const QByteArray boundary = curNode->contentType()->boundary();
            const auto contents = curNode->contents();
            for (KMime::Content *child : contents) {
                resultingData += "\n--" + boundary + '\n';
                const bool changed = unencryptedMessage_helper(child, resultingData, true, recursionLevel + 1);
                if (changed) {
                    somethingChanged = true;
                }
            }
            resultingData += "\n--" + boundary + "--\n\n";
            returnValue = somethingChanged;
        } else if (curNode->bodyIsMessage()) {
            qCDebug(MIMETREEPARSER_LOG) << "Current node is a message, adding the header and then processing the child.";
            if (addHeaders) {
                resultingData += curNode->head() + '\n';
            }

            returnValue = unencryptedMessage_helper(curNode->bodyAsMessage().data(), resultingData, true, recursionLevel + 1);
        } else {
            qCDebug(MIMETREEPARSER_LOG) << "Current node is an ordinary leaf node, adding it as-is.";
            if (addHeaders) {
                resultingData += curNode->head() + '\n';
            }
            resultingData += curNode->body();
            returnValue = false;
        }
    }

    qCDebug(MIMETREEPARSER_LOG) << "(" << recursionLevel << ") done.";
    return returnValue;
}

KMime::Message::Ptr NodeHelper::unencryptedMessage(const KMime::Message::Ptr &originalMessage)
{
    QByteArray resultingData;
    const bool messageChanged = unencryptedMessage_helper(originalMessage.data(), resultingData, true);
    if (messageChanged) {
#if 0
        qCDebug(MIMETREEPARSER_LOG) << "Resulting data is:" << resultingData;
        QFile bla("stripped.mbox");
        bla.open(QIODevice::WriteOnly);
        bla.write(resultingData);
        bla.close();
#endif
        KMime::Message::Ptr newMessage(new KMime::Message);
        newMessage->setContent(resultingData);
        newMessage->parse();
        return newMessage;
    } else {
        return {};
    }
}

QList<KMime::Content *> NodeHelper::attachmentsOfExtraContents() const
{
    QList<KMime::Content *> result;
    for (auto it = mExtraContents.begin(), end = mExtraContents.end(); it != end; ++it) {
        const auto contents = it.value();
        for (auto content : contents) {
            if (KMime::isAttachment(content)) {
                result.push_back(content);
            } else {
                result += content->attachments();
            }
        }
    }
    return result;
}
}

#include "moc_nodehelper.cpp"
