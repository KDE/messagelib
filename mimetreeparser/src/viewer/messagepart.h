/*
   Copyright (c) 2015 Sandro Knauß <sknauss@kde.org>

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

#ifndef __MIMETREEPARSER_MESSAGEPART_H__
#define __MIMETREEPARSER_MESSAGEPART_H__

#include "mimetreeparser_export.h"

#include "mimetreeparser/bodypartformatter.h"
#include "mimetreeparser/util.h"

#include <KMime/Message>

#include <Libkleo/CryptoBackend>
#include <gpgme++/verificationresult.h>
#include <gpgme++/decryptionresult.h>
#include <importresult.h>

#include <QString>
#include <QSharedPointer>

class QTextCodec;

namespace GpgME
{
class ImportResult;
}

namespace KMime
{
class Content;
}

namespace MimeTreeParser
{
class ObjectTreeParser;
class HtmlWriter;
class HTMLBlock;
typedef QSharedPointer<HTMLBlock> HTMLBlockPtr;
class CryptoBodyPartMemento;
class MultiPartAlternativeBodyPartFormatter;
namespace Interface
{
class ObjectTreeSource;
}

class MIMETREEPARSER_EXPORT MessagePart : public Interface::MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool attachment READ isAttachment)
    Q_PROPERTY(bool root READ isRoot)
    Q_PROPERTY(bool isHtml READ isHtml)
    Q_PROPERTY(bool isHidden READ isHidden)
public:
    typedef QSharedPointer<MessagePart> Ptr;
    MessagePart(ObjectTreeParser *otp,
                const QString &text);

    virtual ~MessagePart();

    virtual QString text() const Q_DECL_OVERRIDE;
    void setText(const QString &text);
    void setAttachmentFlag(KMime::Content *node);
    bool isAttachment() const;

    void setIsRoot(bool root);
    bool isRoot() const;

    virtual bool isHtml() const;
    virtual bool isHidden() const;

    PartMetaData *partMetaData();

    /* only a function that should be removed if the refactoring is over */
    virtual void fix() const;
    virtual void copyContentFrom() const;

    void appendSubPart(const Interface::MessagePart::Ptr &messagePart);
    const QVector<Interface::MessagePart::Ptr> &subParts() const;
    bool hasSubParts() const;

    HtmlWriter *htmlWriter() const Q_DECL_OVERRIDE;
    void setHtmlWriter(HtmlWriter *htmlWriter) const Q_DECL_OVERRIDE;

    Interface::ObjectTreeSource *source() const;
    KMime::Content *attachmentNode() const;

protected:
    void parseInternal(KMime::Content *node, bool onlyOneMimePart);
    QString renderInternalText() const;

    QString mText;
    ObjectTreeParser *mOtp;
    PartMetaData mMetaData;

private:
    QVector<Interface::MessagePart::Ptr> mBlocks;

    KMime::Content *mAttachmentNode;
    bool mRoot;
};

class MIMETREEPARSER_EXPORT MimeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MimeMessagePart> Ptr;
    MimeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart);
    virtual ~MimeMessagePart();

    QString text() const Q_DECL_OVERRIDE;

    QString plaintextContent() const Q_DECL_OVERRIDE;
    QString htmlContent() const Q_DECL_OVERRIDE;
private:
    KMime::Content *mNode;
    bool mOnlyOneMimePart;

    friend class AlternativeMessagePart;
};

class MIMETREEPARSER_EXPORT MessagePartList : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MessagePartList> Ptr;
    MessagePartList(MimeTreeParser::ObjectTreeParser *otp);
    virtual ~MessagePartList();

    QString text() const Q_DECL_OVERRIDE;

    QString plaintextContent() const Q_DECL_OVERRIDE;
    QString htmlContent() const Q_DECL_OVERRIDE;
private:
};

enum IconType {
    NoIcon = 0,
    IconExternal,
    IconInline
};

class MIMETREEPARSER_EXPORT TextMessagePart : public MessagePartList
{
    Q_OBJECT
public:
    typedef QSharedPointer<TextMessagePart> Ptr;
    TextMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage);
    virtual ~TextMessagePart();

    KMMsgSignatureState signatureState() const;
    KMMsgEncryptionState encryptionState() const;

    bool decryptMessage() const;

    bool isHidden() const Q_DECL_OVERRIDE;

    bool showLink() const;
    bool showTextFrame() const;

protected:
    KMime::Content *mNode;

private:
    void parseContent();

    KMMsgSignatureState mSignatureState;
    KMMsgEncryptionState mEncryptionState;
    bool mDrawFrame;
    bool mShowLink;
    bool mDecryptMessage;
    bool mIsHidden;

    friend class DefaultRendererPrivate;
    friend class ObjectTreeParser;
};

class MIMETREEPARSER_EXPORT AttachmentMessagePart : public TextMessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AttachmentMessagePart> Ptr;
    AttachmentMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage);
    virtual ~AttachmentMessagePart();

    IconType asIcon() const;
    bool neverDisplayInline() const;
    void setNeverDisplayInline(bool displayInline);
    bool isImage() const;
    void setIsImage(bool image);

    bool isHidden() const Q_DECL_OVERRIDE;

private:
    bool mIsImage;
    bool mNeverDisplayInline;
};

class MIMETREEPARSER_EXPORT HtmlMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<HtmlMessagePart> Ptr;
    HtmlMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, MimeTreeParser::Interface::ObjectTreeSource *source);
    virtual ~HtmlMessagePart();

    QString text() const Q_DECL_OVERRIDE;

    void fix() const Q_DECL_OVERRIDE;
    bool isHtml() const Q_DECL_OVERRIDE;

private:
    KMime::Content *mNode;
    Interface::ObjectTreeSource *mSource;
    QString mBodyHTML;
    QByteArray mCharset;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT AlternativeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AlternativeMessagePart> Ptr;
    AlternativeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, Util::HtmlMode preferredMode);
    virtual ~AlternativeMessagePart();

    QString text() const Q_DECL_OVERRIDE;

    Util::HtmlMode preferredMode() const;

    bool isHtml() const Q_DECL_OVERRIDE;

    QString plaintextContent() const Q_DECL_OVERRIDE;
    QString htmlContent() const Q_DECL_OVERRIDE;

    QList<Util::HtmlMode> availableModes();

    void fix() const Q_DECL_OVERRIDE;
    void copyContentFrom() const Q_DECL_OVERRIDE;
private:
    KMime::Content *mNode;

    Util::HtmlMode mPreferredMode;

    QMap<Util::HtmlMode, KMime::Content *> mChildNodes;
    QMap<Util::HtmlMode, MimeMessagePart::Ptr> mChildParts;

    friend class DefaultRendererPrivate;
    friend class ObjectTreeParser;
    friend class MultiPartAlternativeBodyPartFormatter;
};

class MIMETREEPARSER_EXPORT CertMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<CertMessagePart> Ptr;
    CertMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const Kleo::CryptoBackend::Protocol *cryptoProto, bool autoImport);
    virtual ~CertMessagePart();

    QString text() const Q_DECL_OVERRIDE;

private:
    KMime::Content *mNode;
    bool mAutoImport;
    GpgME::ImportResult mImportResult;
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT EncapsulatedRfc822MessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<EncapsulatedRfc822MessagePart> Ptr;
    EncapsulatedRfc822MessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message);
    virtual ~EncapsulatedRfc822MessagePart();

    QString text() const Q_DECL_OVERRIDE;

    void copyContentFrom() const Q_DECL_OVERRIDE;
    void fix() const Q_DECL_OVERRIDE;
private:
    const KMime::Message::Ptr mMessage;
    KMime::Content *mNode;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT EncryptedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool decryptMessage READ decryptMessage WRITE setDecryptMessage)
    Q_PROPERTY(bool isEncrypted READ isEncrypted)
    Q_PROPERTY(bool passphraseError READ passphraseError)
public:
    typedef QSharedPointer<EncryptedMessagePart> Ptr;
    EncryptedMessagePart(ObjectTreeParser *otp,
                         const QString &text,
                         const Kleo::CryptoBackend::Protocol *cryptoProto,
                         const QString &fromAddress,
                         KMime::Content *node);

    virtual ~EncryptedMessagePart();

    QString text() const Q_DECL_OVERRIDE;

    void setDecryptMessage(bool decrypt);
    bool decryptMessage() const;

    void setIsEncrypted(bool encrypted);
    bool isEncrypted() const;

    bool passphraseError() const;

    void startDecryption(const QByteArray &text, const QTextCodec *aCodec);
    void startDecryption(KMime::Content *data = 0);

    QByteArray mDecryptedData;

    QString plaintextContent() const Q_DECL_OVERRIDE;
    QString htmlContent() const Q_DECL_OVERRIDE;

private:
    /** Handles the dectyptioon of a given content
     * returns true if the decryption was successfull
     * if used in async mode, check if mMetaData.inProgress is true, it inicates a running decryption process.
     */
    bool okDecryptMIME(KMime::Content &data);

protected:
    bool mPassphraseError;
    bool mNoSecKey;
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    QString mFromAddress;
    KMime::Content *mNode;
    bool mDecryptMessage;
    QByteArray mVerifiedText;
    std::vector<GpgME::DecryptionResult::Recipient> mDecryptRecipients;

    friend class DefaultRendererPrivate;
};

class MIMETREEPARSER_EXPORT SignedMessagePart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(bool isSigned READ isSigned)
public:
    typedef QSharedPointer<SignedMessagePart> Ptr;
    SignedMessagePart(ObjectTreeParser *otp,
                      const QString &text,
                      const Kleo::CryptoBackend::Protocol *cryptoProto,
                      const QString &fromAddress,
                      KMime::Content *node);

    virtual ~SignedMessagePart();

    void setIsSigned(bool isSigned);
    bool isSigned() const;

    void startVerification(const QByteArray &text, const QTextCodec *aCodec);
    void startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature);

    QByteArray mDecryptedData;
    std::vector<GpgME::Signature> mSignatures;

    QString plaintextContent() const Q_DECL_OVERRIDE;
    QString htmlContent() const Q_DECL_OVERRIDE;

private:
    /** Handles the verification of data
     * If signature is empty it is handled as inline signature otherwise as detached signature mode.
     * Returns true if the verfication was successfull and the block is signed.
     * If used in async mode, check if mMetaData.inProgress is true, it inicates a running verification process.
     */
    bool okVerify(const QByteArray &data, const QByteArray &signature, KMime::Content *textNode);

    void sigStatusToMetaData();

    void setVerificationResult(const CryptoBodyPartMemento *m, KMime::Content *textNode);
protected:
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    QString mFromAddress;
    KMime::Content *mNode;
    QByteArray mVerifiedText;

    friend EncryptedMessagePart;
    friend class DefaultRendererPrivate;
};

}

#endif //__MIMETREEPARSER_MESSAGEPART_H__
