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
class NodeHelper;
class HTMLBlock;
typedef QSharedPointer<HTMLBlock> HTMLBlockPtr;

namespace Interface
{
class ObjectTreeSource;
}

class MIMETREEPARSER_EXPORT MessagePart : public Interface::MessagePart
{
    Q_OBJECT
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

    PartMetaData *partMetaData();

    /* only a function that should be removed if the refactoring is over */
    virtual void fix() const;
    virtual void copyContentFrom() const;

    void appendSubPart(const Interface::MessagePart::Ptr &messagePart);
    const QVector<Interface::MessagePart::Ptr> &subParts() const;
    bool hasSubParts() const;

protected:
    void parseInternal(KMime::Content *node, bool onlyOneMimePart);
    QString renderInternalText() const;

    HtmlWriter* htmlWriter() const;
    void setHtmlWriter(HtmlWriter *htmlWriter) const;

    QString mText;
    ObjectTreeParser *mOtp;
    ObjectTreeParser *mSubOtp;
    PartMetaData mMetaData;
    KMime::Content *mAttachmentNode;
    bool mRoot;

    Interface::ObjectTreeSource *source() const;
private:
    CSSHelperBase *cssHelper() const;

    KMime::Content *attachmentNode() const;

    QVector<Interface::MessagePart::Ptr> mBlocks;
    bool mIsInternalRoot;

    friend class HtmlRendererPrivate;
};

class MIMETREEPARSER_EXPORT MimeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<MimeMessagePart> Ptr;
    MimeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart);
    virtual ~MimeMessagePart();

    QString text() const Q_DECL_OVERRIDE;

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
    TextMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *node, bool drawFrame, bool showLink, bool decryptMessage, IconType asIcon);
    virtual ~TextMessagePart();


    KMMsgSignatureState signatureState() const;
    KMMsgEncryptionState encryptionState() const;

    bool decryptMessage() const;

private:
    void parseContent();

    KMime::Content *mNode;
    KMMsgSignatureState mSignatureState;
    KMMsgEncryptionState mEncryptionState;
    bool mDrawFrame;
    bool mShowLink;
    bool mDecryptMessage;
    IconType mAsIcon;

    friend class HtmlRendererPrivate;
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

private:
    QString processHtml(const QString &htmlSource, QString &extraHead);
    KMime::Content *mNode;
    Interface::ObjectTreeSource *mSource;
    QString mBodyHTML;
    QByteArray mCharset;

    friend class HtmlRendererPrivate;
};

class MIMETREEPARSER_EXPORT AlternativeMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<AlternativeMessagePart> Ptr;
    AlternativeMessagePart(MimeTreeParser::ObjectTreeParser *otp, KMime::Content *textNode, KMime::Content *htmlNode);
    virtual ~AlternativeMessagePart();

    QString text() const Q_DECL_OVERRIDE;

    void setViewHtml(bool html);
    bool viewHtml() const;
    void fix() const Q_DECL_OVERRIDE;
    void copyContentFrom() const Q_DECL_OVERRIDE;
private:
    KMime::Content *mTextNode;
    KMime::Content *mHTMLNode;

    MimeMessagePart::Ptr mTextPart;
    MimeMessagePart::Ptr mHTMLPart;
    bool mViewHtml;
    friend class HtmlRendererPrivate;
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
    friend class HtmlRendererPrivate;
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

    friend class HtmlRendererPrivate;
};

class MIMETREEPARSER_EXPORT CryptoMessagePart : public MessagePart
{
    Q_OBJECT
public:
    typedef QSharedPointer<CryptoMessagePart> Ptr;
    CryptoMessagePart(ObjectTreeParser *otp,
                      const QString &text,
                      const Kleo::CryptoBackend::Protocol *cryptoProto,
                      const QString &fromAddress,
                      KMime::Content *node);

    virtual ~CryptoMessagePart();

    void setDecryptMessage(bool decrypt);
    bool decryptMessage() const;

    void setIsEncrypted(bool encrypted);
    bool isEncrypted() const;

    void setIsSigned(bool isSigned);
    bool isSigned() const;

    bool passphraseError() const;

    void startDecryption(const QByteArray &text, const QTextCodec *aCodec);
    void startDecryption(KMime::Content *data = 0);
    void startVerification(const QByteArray &text, const QTextCodec *aCodec);
    void startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature);

    QByteArray mDecryptedData;
    std::vector<GpgME::Signature> mSignatures;

private:
    /** Handles the dectyptioon of a given content
     * returns true if the decryption was successfull
     * if used in async mode, check if mMetaData.inProgress is true, it inicates a running decryption process.
     */
    bool okDecryptMIME(KMime::Content &data);

    /** Handles the verification of data
     * If signature is empty it is handled as inline signature otherwise as detached signature mode.
     * Returns true if the verfication was successfull and the block is signed.
     * If used in async mode, check if mMetaData.inProgress is true, it inicates a running verification process.
     */
    bool okVerify(const QByteArray &data, const QByteArray &signature);

    void sigStatusToMetaData();
protected:
    bool mPassphraseError;
    bool mNoSecKey;
    const Kleo::CryptoBackend::Protocol *mCryptoProto;
    QString mFromAddress;
    KMime::Content *mNode;
    bool mDecryptMessage;
    QByteArray mVerifiedText;
    std::vector<GpgME::DecryptionResult::Recipient> mDecryptRecipients;

    friend class HtmlRendererPrivate;
};

}

#endif //__MIMETREEPARSER_MESSAGEPART_H__
