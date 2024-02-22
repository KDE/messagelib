/*
   SPDX-FileCopyrightText: 2015 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"

#include "mimetreeparser/bodypartformatter.h"
#include "mimetreeparser/util.h"

#include <KMime/Message>

#include <gpgme++/decryptionresult.h>
#include <gpgme++/importresult.h>
#include <gpgme++/key.h>
#include <gpgme++/verificationresult.h>

#include <QSharedPointer>
#include <QString>

#include <memory>

namespace GpgME
{
class ImportResult;
}

namespace QGpgME
{
class Protocol;
}

namespace KMime
{
class Content;
}

namespace Kleo
{
class KeyCache;
}

namespace MimeTreeParser
{
class CompositeMemento;
class CryptoBodyPartMemento;
class MessagePartPrivate;
namespace Interface
{
class ObjectTreeSource;
}
/**
 * @brief The MessagePart class
 */
class MIMETREEPARSER_EXPORT MessagePart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString plaintextContent READ plaintextContent)
    Q_PROPERTY(QString htmlContent READ htmlContent)
    Q_PROPERTY(bool isAttachment READ isAttachment)
    Q_PROPERTY(bool root READ isRoot)
    Q_PROPERTY(bool isHtml READ isHtml)
    Q_PROPERTY(bool isImage READ isImage CONSTANT)
    Q_PROPERTY(bool neverDisplayInline READ neverDisplayInline CONSTANT)
    Q_PROPERTY(QString attachmentIndex READ attachmentIndex CONSTANT)
    Q_PROPERTY(QString link READ attachmentLink CONSTANT)
public:
    using Ptr = QSharedPointer<MessagePart>;
    MessagePart(ObjectTreeParser *otp, const QString &text);
    ~MessagePart() override;

    void setParentPart(MessagePart *parentPart);
    [[nodiscard]] MessagePart *parentPart() const;

    [[nodiscard]] virtual QString text() const;
    void setText(const QString &text);

    [[nodiscard]] virtual QString plaintextContent() const;
    [[nodiscard]] virtual QString htmlContent() const;

    /** The KMime::Content* node that's represented by this part.
     *  Can be @c nullptr, e.g. for sub-parts of an inline signed body part.
     */
    [[nodiscard]] KMime::Content *content() const;
    void setContent(KMime::Content *node);

    /** The KMime::Content* node that's the source of this part.
     *  This is not necessarily the same as content(), for example for
     *  broken-up multipart nodes.
     */
    [[nodiscard]] KMime::Content *attachmentContent() const;
    void setAttachmentContent(KMime::Content *node);
    [[nodiscard]] bool isAttachment() const;
    /** @see KMime::Content::index() */
    [[nodiscard]] QString attachmentIndex() const;
    /** @see NodeHelper::asHREF */
    [[nodiscard]] QString attachmentLink() const;

    /** Returns a string representation of an URL that can be used
     *  to invoke a BodyPartURLHandler for this body part.
     */
    [[nodiscard]] QString makeLink(const QString &path) const;

    void setIsRoot(bool root);
    [[nodiscard]] bool isRoot() const;

    virtual bool isHtml() const;

    [[nodiscard]] bool neverDisplayInline() const;
    void setNeverDisplayInline(bool displayInline);
    [[nodiscard]] bool isImage() const;
    void setIsImage(bool image);

    PartMetaData *partMetaData() const;

    Interface::BodyPartMemento *memento() const;
    void setMemento(Interface::BodyPartMemento *memento);

    /* only a function that should be removed if the refactoring is over */
    virtual void fix() const;

    void appendSubPart(const MessagePart::Ptr &messagePart);
    const QList<MessagePart::Ptr> &subParts() const;
    [[nodiscard]] bool hasSubParts() const;
    void clearSubParts();

    Interface::ObjectTreeSource *source() const;
    NodeHelper *nodeHelper() const;

    [[nodiscard]] virtual bool hasHeader(const char *headerType) const;
    virtual const KMime::Headers::Base *header(const char *headerType) const;
    virtual QList<KMime::Headers::Base *> headers(const char *headerType) const;

protected:
    void parseInternal(KMime::Content *node, bool onlyOneMimePart);
    [[nodiscard]] QString renderInternalText() const;

    ObjectTreeParser *mOtp = nullptr;

private:
    std::unique_ptr<MessagePartPrivate> d;
};
/**
 * @brief The MimeMessagePart class
 */
class MIMETREEPARSER_EXPORT MimeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<MimeMessagePart>;
    MimeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart);
    ~MimeMessagePart() override;

    [[nodiscard]] QString text() const override;

    [[nodiscard]] QString plaintextContent() const override;
    [[nodiscard]] QString htmlContent() const override;

private:
    const bool mOnlyOneMimePart;
};
/**
 * @brief The MessagePartList class
 */
class MIMETREEPARSER_EXPORT MessagePartList : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<MessagePartList>;
    explicit MessagePartList(MimeTreeParser::ObjectTreeParser *otp);
    ~MessagePartList() override;

    [[nodiscard]] QString text() const override;

    [[nodiscard]] QString plaintextContent() const override;
    [[nodiscard]] QString htmlContent() const override;
};

enum IconType { NoIcon = 0, IconExternal, IconInline };
/**
 * @brief The TextMessagePart class
 */
class MIMETREEPARSER_EXPORT TextMessagePart : public MessagePartList
{
    Q_OBJECT
    Q_PROPERTY(bool showLink READ showLink CONSTANT)
    Q_PROPERTY(bool isFirstTextPart READ isFirstTextPart CONSTANT)
    Q_PROPERTY(bool hasLabel READ hasLabel CONSTANT)
    Q_PROPERTY(QString label READ label CONSTANT)
    Q_PROPERTY(QString comment READ comment CONSTANT)
public:
    using Ptr = QSharedPointer<TextMessagePart>;
    TextMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage);
    ~TextMessagePart() override;

    KMMsgSignatureState signatureState() const;
    KMMsgEncryptionState encryptionState() const;

    [[nodiscard]] bool decryptMessage() const;

    [[nodiscard]] bool showLink() const;
    [[nodiscard]] bool isFirstTextPart() const;
    [[nodiscard]] bool hasLabel() const;

    /** The attachment filename, or the closest approximation thereof we have. */
    [[nodiscard]] QString label() const;
    /** A description of this attachment, if provided. */
    [[nodiscard]] QString comment() const;
    /** Temporary file containing the part content. */
    [[nodiscard]] QString temporaryFilePath() const;

private:
    MIMETREEPARSER_NO_EXPORT void parseContent();

    KMMsgSignatureState mSignatureState;
    KMMsgEncryptionState mEncryptionState;
    const bool mDecryptMessage;
};
/**
 * @brief The AttachmentMessagePart class
 */
class MIMETREEPARSER_EXPORT AttachmentMessagePart : public TextMessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<AttachmentMessagePart>;
    AttachmentMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage);
    ~AttachmentMessagePart() override;
};
/**
 * @brief The HtmlMessagePart class
 */
class MIMETREEPARSER_EXPORT HtmlMessagePart : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<HtmlMessagePart>;
    HtmlMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, MimeTreeParser::Interface::ObjectTreeSource *source);
    ~HtmlMessagePart() override;

    [[nodiscard]] QString text() const override;
    [[nodiscard]] QString plaintextContent() const override;

    void fix() const override;
    [[nodiscard]] bool isHtml() const override;

    [[nodiscard]] QString bodyHtml() const;

private:
    QString mBodyHTML;
    QByteArray mCharset;
};
/**
 * @brief The AlternativeMessagePart class
 */
class MIMETREEPARSER_EXPORT AlternativeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<AlternativeMessagePart>;
    AlternativeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, Util::HtmlMode preferredMode);
    ~AlternativeMessagePart() override;

    [[nodiscard]] QString text() const override;

    [[nodiscard]] Util::HtmlMode preferredMode() const;
    void setPreferredMode(Util::HtmlMode preferredMode);

    [[nodiscard]] bool isHtml() const override;

    [[nodiscard]] QString plaintextContent() const override;
    [[nodiscard]] QString htmlContent() const override;

    [[nodiscard]] QList<Util::HtmlMode> availableModes();

    void fix() const override;

    const QMap<Util::HtmlMode, MimeMessagePart::Ptr> &childParts() const;

private:
    Util::HtmlMode mPreferredMode;

    QMap<Util::HtmlMode, KMime::Content *> mChildNodes;
    QMap<Util::HtmlMode, MimeMessagePart::Ptr> mChildParts;
};
/**
 * @brief The CertMessagePart class
 */
class MIMETREEPARSER_EXPORT CertMessagePart : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<CertMessagePart>;
    CertMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const QGpgME::Protocol *cryptoProto, bool autoImport);
    ~CertMessagePart() override;

    [[nodiscard]] QString text() const override;

    const GpgME::ImportResult &importResult() const;

private:
    bool mAutoImport;
    GpgME::ImportResult mImportResult;
    const QGpgME::Protocol *mCryptoProto;
};
/**
 * @brief The EncapsulatedRfc822MessagePart class
 */
class MIMETREEPARSER_EXPORT EncapsulatedRfc822MessagePart : public MessagePart
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<EncapsulatedRfc822MessagePart>;
    EncapsulatedRfc822MessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message);
    ~EncapsulatedRfc822MessagePart() override;

    [[nodiscard]] QString text() const override;

    void fix() const override;

    const KMime::Message::Ptr message() const;

private:
    const KMime::Message::Ptr mMessage;
};
/**
 * @brief The EncryptedMessagePart class
 */
class MIMETREEPARSER_EXPORT EncryptedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool decryptMessage READ decryptMessage WRITE setDecryptMessage)
    Q_PROPERTY(bool isEncrypted READ isEncrypted)
    Q_PROPERTY(bool isNoSecKey READ isNoSecKey)
    Q_PROPERTY(bool passphraseError READ passphraseError)
public:
    using Ptr = QSharedPointer<EncryptedMessagePart>;
    EncryptedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    ~EncryptedMessagePart() override;

    [[nodiscard]] QString text() const override;

    void setDecryptMessage(bool decrypt);
    [[nodiscard]] bool decryptMessage() const;

    void setIsEncrypted(bool encrypted);
    [[nodiscard]] bool isEncrypted() const;

    [[nodiscard]] bool isDecryptable() const;

    [[nodiscard]] bool isNoSecKey() const;
    [[nodiscard]] bool passphraseError() const;

    void startDecryption(const QByteArray &text, QByteArrayView aCodec);
    void startDecryption(KMime::Content *data = nullptr);

    void setMementoName(const QByteArray &name);
    [[nodiscard]] QByteArray mementoName() const;

    [[nodiscard]] QString plaintextContent() const override;
    [[nodiscard]] QString htmlContent() const override;

    const QGpgME::Protocol *cryptoProto() const;
    [[nodiscard]] QString fromAddress() const;

    const std::vector<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key>> &decryptRecipients() const;

    [[nodiscard]] bool hasHeader(const char *headerType) const override;
    const KMime::Headers::Base *header(const char *headerType) const override;
    QList<KMime::Headers::Base *> headers(const char *headerType) const override;

    QByteArray mDecryptedData;

private:
    /** Handles the decryption of a given content
     * returns true if the decryption was successful
     * if used in async mode, check if mMetaData.inPogress is true, it initiates a running decryption process.
     */
    [[nodiscard]] bool okDecryptMIME(KMime::Content &data);

protected:
    bool mPassphraseError;
    bool mNoSecKey;
    bool mDecryptMessage;
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    QByteArray mVerifiedText;
    QByteArray mMementoName;
    std::vector<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key>> mDecryptRecipients;
    std::shared_ptr<const Kleo::KeyCache> mKeyCache;

    friend class EncryptedBodyPartFormatter;
};
/**
 * @brief The SignedMessagePart class
 */
class MIMETREEPARSER_EXPORT SignedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool isSigned READ isSigned)
public:
    using Ptr = QSharedPointer<SignedMessagePart>;
    SignedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node);

    ~SignedMessagePart() override;

    void setIsSigned(bool isSigned);
    [[nodiscard]] bool isSigned() const;

    void startVerification(const QByteArray &text, QByteArrayView aCodec);
    void startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature);

    void setMementoName(const QByteArray &name);
    [[nodiscard]] QByteArray mementoName() const;

    QByteArray mDecryptedData;
    std::vector<GpgME::Signature> mSignatures;

    [[nodiscard]] QString plaintextContent() const override;
    [[nodiscard]] QString htmlContent() const override;

    const QGpgME::Protocol *cryptoProto() const;
    [[nodiscard]] QString fromAddress() const;

    [[nodiscard]] bool hasHeader(const char *headerType) const override;
    const KMime::Headers::Base *header(const char *headerType) const override;
    [[nodiscard]] QList<KMime::Headers::Base *> headers(const char *headerType) const override;

private:
    /** Handles the verification of data
     * If signature is empty it is handled as inline signature otherwise as detached signature mode.
     * Returns true if the verification was successful and the block is signed.
     * If used in async mode, check if mMetaData.inProgress is true, it initiates a running verification process.
     */
    [[nodiscard]] MIMETREEPARSER_NO_EXPORT bool okVerify(const QByteArray &data, const QByteArray &signature, KMime::Content *textNode);

    MIMETREEPARSER_NO_EXPORT void sigStatusToMetaData();

    MIMETREEPARSER_NO_EXPORT void setVerificationResult(const CompositeMemento *m, KMime::Content *textNode);

protected:
    const QGpgME::Protocol *mCryptoProto;
    QString mFromAddress;
    QByteArray mVerifiedText;
    QByteArray mMementoName;
    std::shared_ptr<const Kleo::KeyCache> mKeyCache;

    friend EncryptedMessagePart;
};
}
