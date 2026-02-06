/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include "mimetreeparser/enums.h"
#include "mimetreeparser/partmetadata.h"

#include <KMime/Headers>
#include <KMime/Message>

#include <QList>
#include <QMap>
#include <QPointer>
#include <QSet>

class QUrl;

namespace MimeTreeParser
{
class AttachmentTemporaryFilesDirs;
class MessagePart;
using MessagePartPtr = QSharedPointer<MessagePart>;
namespace Interface
{
class BodyPartMemento;
}
}

namespace MimeTreeParser
{
/**
 * \class MimeTreeParser::NodeHelper
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/NodeHelper
 *
 * \author Andras Mantia <andras@kdab.net>
 */
class MIMETREEPARSER_EXPORT NodeHelper : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor.
     */
    NodeHelper();

    /*!
     */
    ~NodeHelper() override;

    /*!
     */
    void setNodeProcessed(const KMime::Content *node, bool recurse);
    /*!
     */
    void setNodeUnprocessed(const KMime::Content *node, bool recurse);
    /*!
     */
    [[nodiscard]] bool nodeProcessed(const KMime::Content *node) const;
    /*!
     */
    void clear();
    /*!
     */
    void forceCleanTempFiles();

    /*!
     */
    void setEncryptionState(const KMime::Content *node, const KMMsgEncryptionState state);
    /*!
     */
    [[nodiscard]] KMMsgEncryptionState encryptionState(const KMime::Content *node) const;

    /*!
     */
    void setSignatureState(const KMime::Content *node, const KMMsgSignatureState state);
    /*!
     */
    [[nodiscard]] KMMsgSignatureState signatureState(const KMime::Content *node) const;

    /*!
     */
    [[nodiscard]] KMMsgSignatureState overallSignatureState(KMime::Content *node) const;
    /*!
     */
    [[nodiscard]] KMMsgEncryptionState overallEncryptionState(KMime::Content *node) const;

    /*!
     */
    void setPartMetaData(const KMime::Content *node, const PartMetaData &metaData);
    /*!
     */
    [[nodiscard]] PartMetaData partMetaData(const KMime::Content *node);

    /**
     *  Set the 'Content-Type' by mime-magic from the contents of the body.
     *  If autoDecode is true the decoded body will be used for mime type
     *  determination (this does not change the body itself).
     */
    static void magicSetType(KMime::Content *node, bool autoDecode = true);

    /*!
     */
    void clearOverrideHeaders();
    /*!
     */
    void registerOverrideHeader(const KMime::Content *message, MessagePartPtr part);
    /*!
     */
    [[nodiscard]] bool hasMailHeader(const char *header, const KMime::Content *message) const;
    /*!
     */
    [[nodiscard]] QList<MessagePartPtr> messagePartsOfMailHeader(const char *header, const KMime::Content *message) const;
    /*!
     */
    KMime::Headers::Base const *mailHeaderAsBase(const char *header, const KMime::Content *message) const;
    /*!
     */
    [[nodiscard]] QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header, const KMime::Content *message) const;
    /*!
     */
    [[nodiscard]] QList<KMime::Headers::Base *> headers(const char *header, const KMime::Content *message);
    /*!
     */
    [[nodiscard]] QDateTime dateHeader(const KMime::Content *message) const;

    /** Attach an extra node to an existing node */
    void attachExtraContent(const KMime::Content *topLevelNode, std::unique_ptr<KMime::Content> &&content);

    /*!
     */
    void cleanExtraContent(const KMime::Content *topLevelNode);

    /** Get the extra nodes attached to the @param topLevelNode and all sub-nodes of @param topLevelNode */
    [[nodiscard]] QList<KMime::Content *> extraContents(const KMime::Content *topLevelNode) const;

    /** Return a modified message (node tree) starting from @param topLevelNode that has the original nodes and the extra nodes.
        The caller has the responsibility to delete the new message.
     */
    [[nodiscard]] KMime::Message *messageWithExtraContent(KMime::Content *topLevelNode);

    /** Get a codec suitable for this message part */
    [[nodiscard]] QByteArray codecName(const KMime::Content *node) const;

    /** Set the charset the user selected for the message to display */
    void setOverrideCodec(const KMime::Content *node, const QByteArray &codec);

    /*!
     */
    Interface::BodyPartMemento *bodyPartMemento(const KMime::Content *node, const QByteArray &which) const;

    /*!
     */
    void setBodyPartMemento(const KMime::Content *node, const QByteArray &which, Interface::BodyPartMemento *memento);

    // A flag to remember if the node was embedded. This is useful for attachment nodes, the reader
    // needs to know if they were displayed inline or not.
    [[nodiscard]] bool isNodeDisplayedEmbedded(const KMime::Content *node) const;
    void setNodeDisplayedEmbedded(const KMime::Content *node, bool displayedEmbedded);

    // Same as above, but this time determines if the node was hidden or not
    [[nodiscard]] bool isNodeDisplayedHidden(const KMime::Content *node) const;
    void setNodeDisplayedHidden(const KMime::Content *node, bool displayedHidden);

    /**
     * Writes the given message part to a temporary file and returns the
     * name of this file or QString() if writing failed.
     */
    QString writeNodeToTempFile(const KMime::Content *node);

    [[nodiscard]] QString writeFileToTempFile(const KMime::Content *node, const QString &filename);

    /**
     * Returns the temporary file path and name where this node was saved, or an empty url
     * if it wasn't saved yet with writeNodeToTempFile()
     */
    [[nodiscard]] QUrl tempFileUrlFromNode(const KMime::Content *node);

    /**
     * Creates a temporary dir for saving attachments, etc.
     * Will be automatically deleted when another message is viewed.
     * @param param Optional part of the directory name.
     */
    [[nodiscard]] QString createTempDir(const QString &param = QString());

    /**
     * Cleanup the attachment temp files
     */
    void removeTempFiles();

    /**
     * Add a file to the list of managed temporary files
     */
    void addTempFile(const QString &file);

    // Get a href in the form attachment:<nodeId>?place=<place>, used by ObjectTreeParser and
    // UrlHandlerManager.
    /*!
     */
    [[nodiscard]] QString asHREF(const KMime::Content *node, const QString &place) const;
    /*!
     */
    [[nodiscard]] KMime::Content *fromHREF(const std::shared_ptr<KMime::Message> &mMessage, const QUrl &href) const;

    // Overload which creates a URL without the query part. Used by MessagePart::makeLink.
    /*!
     */
    [[nodiscard]] QString asHREF(const KMime::Content *node) const;

    /**
     * @return true if this node is a child or an encapsulated message
     */
    [[nodiscard]] static bool isInEncapsulatedMessage(const KMime::Content *node);

    /**
     * Returns the charset for the given node. If no charset is specified
     * for the node, the defaultCharset() is returned.
     */
    [[nodiscard]] static QByteArray charset(const KMime::Content *node);

    /**
     * Returns a usable filename for a node, that can be the filename from the
     * content disposition header, or if that one is empty, the name from the
     * content type header.
     */
    [[nodiscard]] static QString fileName(const KMime::Content *node);

    /**
     * Fixes an encoding received by a KDE function and returns the proper,
     * MIME-compliant encoding name instead.
     * @see encodingForName
     */
    [[nodiscard]] static QString fixEncoding(const QString &encoding); // TODO(Andras) move to a utility class?

    /**
     * Drop-in replacement for KCharsets::encodingForName(). The problem with
     * the KCharsets function is that it returns "human-readable" encoding names
     * like "ISO 8859-15" instead of valid encoding names like "ISO-8859-15".
     * This function fixes this by replacing whitespace with a hyphen.
     */
    [[nodiscard]] static QString encodingForName(const QString &descriptiveName); // TODO(Andras) move to a utility class?

    /**
     * Return a list of the supported encodings
     */
    [[nodiscard]] static QStringList supportedEncodings(); // TODO(Andras) move to a utility class?

    /*!
     */
    [[nodiscard]] QString fromAsString(const KMime::Content *node) const;

    /*!
     */
    [[nodiscard]] KMime::Content *decryptedNodeForContent(const KMime::Content *content) const;

    /**
     * This function returns the unencrypted message that is based on @p originalMessage.
     * All encrypted MIME parts are removed and replaced by their decrypted plain-text versions.
     * Encrypted parts that are within signed parts are not replaced, since that would invalidate
     * the signature.
     *
     * This only works if the message was run through ObjectTreeParser::parseObjectTree() with the
     * current NodeHelper before, because parseObjectTree() actually decrypts the message and stores
     * the decrypted nodes by calling attachExtraContent().
     *
     * @return the unencrypted message or an invalid pointer if the original message didn't contain
     *         a part that needed to be modified.
     */
    [[nodiscard]] std::shared_ptr<KMime::Message> unencryptedMessage(const std::shared_ptr<KMime::Message> &originalMessage);

    /**
     * Returns a list of attachments of attached extra content nodes.
     * This is mainly useful is order to get attachments of encrypted messages.
     * Note that this does not include attachments from the primary node tree.
     * @see KMime::Content::attachments().
     */
    [[nodiscard]] QList<KMime::Content *> attachmentsOfExtraContents() const;

    [[nodiscard]] QString extractAttachmentIndex(const QString &path) const;
Q_SIGNALS:
    /*!
     */
    void update(MimeTreeParser::UpdateMode);

private:
    Q_DISABLE_COPY(NodeHelper)
    bool unencryptedMessage_helper(const KMime::Content *node, QByteArray &resultingData, bool addHeaders, int recursionLevel = 1);

    MIMETREEPARSER_NO_EXPORT void mergeExtraNodes(KMime::Content *node);
    MIMETREEPARSER_NO_EXPORT void cleanFromExtraNodes(KMime::Content *node);

    /** Creates a persistent index string that bridges the gap between the
        permanent nodes and the temporary ones.

        Used internally for robust indexing.
    **/
    [[nodiscard]] QString persistentIndex(const KMime::Content *node) const;

    /** Translates the persistentIndex into a node back

        node: any node of the actually message to what the persistentIndex is interpreded
    **/
    KMime::Content *contentFromIndex(KMime::Content *node, const QString &persistentIndex) const;

private:
    QList<const KMime::Content *> mProcessedNodes;
    QList<const KMime::Content *> mNodesUnderProcess;
    QMap<const KMime::Content *, KMMsgEncryptionState> mEncryptionState;
    QMap<const KMime::Content *, KMMsgSignatureState> mSignatureState;
    QSet<const KMime::Content *> mDisplayEmbeddedNodes;
    QSet<const KMime::Content *> mDisplayHiddenNodes;
    QMap<const KMime::Content *, QByteArray> mOverrideCodecs;
    QMap<QString, QMap<QByteArray, Interface::BodyPartMemento *>> mBodyPartMementoMap;
    QMap<const KMime::Content *, PartMetaData> mPartMetaDatas;
    QMap<const KMime::Content *, QList<KMime::Content *>> mExtraContents;
    QPointer<AttachmentTemporaryFilesDirs> mAttachmentFilesDir;
    QMap<const KMime::Content *, QList<MessagePartPtr>> mHeaderOverwrite;
    QList<QPointer<AttachmentTemporaryFilesDirs>> mListAttachmentTemporaryDirs;
    friend class NodeHelperTest;
};
}
