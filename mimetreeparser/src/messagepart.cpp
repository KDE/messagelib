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

#include "messagepart.h"
#include "mimetreeparser_debug.h"
#include "cryptohelper.h"
#include "objecttreeparser.h"
#include "job/qgpgmejobexecutor.h"
#include "memento/cryptobodypartmemento.h"
#include "memento/decryptverifybodypartmemento.h"
#include "memento/verifydetachedbodypartmemento.h"
#include "memento/verifyopaquebodypartmemento.h"

#include "bodyformatter/utils.h"

#include <KMime/Content>

#include <QGpgME/DN>
#include <QGpgME/Protocol>
#include <QGpgME/ImportJob>
#include <QGpgME/KeyListJob>
#include <QGpgME/VerifyDetachedJob>
#include <QGpgME/VerifyOpaqueJob>

#include <gpgme++/key.h>
#include <gpgme++/keylistresult.h>
#include <gpgme.h>

#include <KLocalizedString>

#include <QTextCodec>
#include <QUrl>

using namespace MimeTreeParser;

//------MessagePart-----------------------
namespace MimeTreeParser {
class MessagePartPrivate
{
public:
    MessagePart *mParentPart = nullptr;
    QVector<MessagePart::Ptr> mBlocks;
    KMime::Content *mNode = nullptr;
    KMime::Content *mAttachmentNode = nullptr;
    QString mText;
    PartMetaData mMetaData;
    bool mRoot = false;
    bool mIsImage = false;
    bool mNeverDisplayInline = false;
};
}

MessagePart::MessagePart(ObjectTreeParser *otp, const QString &text)
    : mOtp(otp)
    , d(new MessagePartPrivate)
{
    d->mText = text;
}

MessagePart::~MessagePart() = default;

MessagePart *MessagePart::parentPart() const
{
    return d->mParentPart;
}

void MessagePart::setParentPart(MessagePart *parentPart)
{
    d->mParentPart = parentPart;
}

QString MessagePart::htmlContent() const
{
    return text();
}

QString MessagePart::plaintextContent() const
{
    return text();
}

PartMetaData *MessagePart::partMetaData() const
{
    return &d->mMetaData;
}

Interface::BodyPartMemento *MessagePart::memento() const
{
    return nodeHelper()->bodyPartMemento(content(), "__plugin__");
}

void MessagePart::setMemento(Interface::BodyPartMemento *memento)
{
    nodeHelper()->setBodyPartMemento(content(), "__plugin__", memento);
}

KMime::Content *MessagePart::content() const
{
    return d->mNode;
}

void MessagePart::setContent(KMime::Content *node)
{
    d->mNode = node;
}

KMime::Content *MessagePart::attachmentContent() const
{
    return d->mAttachmentNode;
}

void MessagePart::setAttachmentContent(KMime::Content *node)
{
    d->mAttachmentNode = node;
}

bool MessagePart::isAttachment() const
{
    return d->mAttachmentNode;
}

QString MessagePart::attachmentIndex() const
{
    return attachmentContent()->index().toString();
}

QString MessagePart::attachmentLink() const
{
    return mOtp->nodeHelper()->asHREF(content(), QStringLiteral("body"));
}

QString MessagePart::makeLink(const QString &path) const
{
    // FIXME: use a PRNG for the first arg, instead of a serial number
    static int serial = 0;
    if (path.isEmpty()) {
        return {};
    }
    return QStringLiteral("x-kmail:/bodypart/%1/%2/%3")
           .arg(serial++).arg(content()->index().toString(), QString::fromLatin1(QUrl::toPercentEncoding(path, "/")));
}

void MessagePart::setIsRoot(bool root)
{
    d->mRoot = root;
}

bool MessagePart::isRoot() const
{
    return d->mRoot;
}

QString MessagePart::text() const
{
    return d->mText;
}

void MessagePart::setText(const QString &text)
{
    d->mText = text;
}

bool MessagePart::isHtml() const
{
    return false;
}

Interface::ObjectTreeSource *MessagePart::source() const
{
    Q_ASSERT(mOtp);
    return mOtp->mSource;
}

NodeHelper *MessagePart::nodeHelper() const
{
    Q_ASSERT(mOtp);
    return mOtp->nodeHelper();
}

void MessagePart::parseInternal(KMime::Content *node, bool onlyOneMimePart)
{
    auto subMessagePart = mOtp->parseObjectTreeInternal(node, onlyOneMimePart);
    d->mRoot = subMessagePart->isRoot();
    const QVector<MessagePart::Ptr> subParts = subMessagePart->subParts();
    for (const auto &part : subParts) {
        appendSubPart(part);
    }
}

QString MessagePart::renderInternalText() const
{
    QString text;
    foreach (const auto &mp, subParts()) {
        text += mp->text();
    }
    return text;
}

void MessagePart::fix() const
{
    foreach (const auto &mp, subParts()) {
        const auto m = mp.dynamicCast<MessagePart>();
        if (m) {
            m->fix();
        }
    }
}

void MessagePart::appendSubPart(const MessagePart::Ptr &messagePart)
{
    messagePart->setParentPart(this);
    d->mBlocks.append(messagePart);
}

const QVector<MessagePart::Ptr> &MessagePart::subParts() const
{
    return d->mBlocks;
}

bool MessagePart::hasSubParts() const
{
    return !d->mBlocks.isEmpty();
}

void MessagePart::clearSubParts()
{
    d->mBlocks.clear();
}

bool MessagePart::neverDisplayInline() const
{
    return d->mNeverDisplayInline;
}

void MessagePart::setNeverDisplayInline(bool displayInline)
{
    d->mNeverDisplayInline = displayInline;
}

bool MessagePart::isImage() const
{
    return d->mIsImage;
}

void MessagePart::setIsImage(bool image)
{
    d->mIsImage = image;
}

bool MessagePart::hasHeader(const char *header) const
{
    Q_UNUSED(header);
    return false;
}

KMime::Headers::Base *MimeTreeParser::MessagePart::header(const char *header) const
{
    Q_UNUSED(header);
    return nullptr;
}

//-----MessagePartList----------------------
MessagePartList::MessagePartList(ObjectTreeParser *otp)
    : MessagePart(otp, QString())
{
}

MessagePartList::~MessagePartList()
{
}

QString MessagePartList::text() const
{
    return renderInternalText();
}

QString MessagePartList::plaintextContent() const
{
    return QString();
}

QString MessagePartList::htmlContent() const
{
    return QString();
}

//-----TextMessageBlock----------------------

TextMessagePart::TextMessagePart(ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage)
    : MessagePartList(otp)
    , mDecryptMessage(decryptMessage)
{
    if (!node) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }

    setContent(node);

    parseContent();
}

TextMessagePart::~TextMessagePart()
{
}

bool TextMessagePart::decryptMessage() const
{
    return mDecryptMessage;
}

void TextMessagePart::parseContent()
{
    const auto aCodec = mOtp->codecFor(content());
    const QString &fromAddress = mOtp->nodeHelper()->fromAsString(content());
    mSignatureState = KMMsgNotSigned;
    mEncryptionState = KMMsgNotEncrypted;
    const auto blocks = prepareMessageForDecryption(content()->decodedContent());

    const auto cryptProto = QGpgME::openpgp();

    if (!blocks.isEmpty()) {
        /* The (overall) signature/encrypted status is broken
         * if one unencrypted part is at the beginning or in the middle
         * because mailmain adds an unencrypted part at the end this should not break the overall status
         *
         * That's why we first set the tmp status and if one crypted/signed block comes afterwards, than
         * the status is set to unencrypted
         */
        bool fullySignedOrEncrypted = true;
        bool fullySignedOrEncryptedTmp = true;

        int blockIndex = -1;
        for (const auto &block : blocks) {
            blockIndex += 1;
            if (!fullySignedOrEncryptedTmp) {
                fullySignedOrEncrypted = false;
            }

            if (block.type() == NoPgpBlock && !block.text().trimmed().isEmpty()) {
                fullySignedOrEncryptedTmp = false;
                appendSubPart(MessagePart::Ptr(new MessagePart(mOtp, aCodec->toUnicode(block.text()))));
            } else if (block.type() == PgpMessageBlock) {
                EncryptedMessagePart::Ptr mp(new EncryptedMessagePart(mOtp, QString(), cryptProto, fromAddress, nullptr));
                mp->setDecryptMessage(decryptMessage());
                mp->setIsEncrypted(true);
                mp->setMementoName(mp->mementoName() + "-" + nodeHelper()->asHREF(content(), QString::number(blockIndex)).toLocal8Bit());
                appendSubPart(mp);
                if (!decryptMessage()) {
                    continue;
                }
                mp->startDecryption(block.text(), aCodec);
                if (mp->partMetaData()->inProgress) {
                    continue;
                }
            } else if (block.type() == ClearsignedBlock) {
                SignedMessagePart::Ptr mp(new SignedMessagePart(mOtp, QString(), cryptProto, fromAddress, nullptr));
                mp->setMementoName(mp->mementoName() + "-" + nodeHelper()->asHREF(content(), QString::number(blockIndex)).toLocal8Bit());
                appendSubPart(mp);
                mp->startVerification(block.text(), aCodec);
            } else {
                continue;
            }

            const auto mp = subParts().last().staticCast<MessagePart>();
            const PartMetaData *messagePart(mp->partMetaData());

            if (!messagePart->isEncrypted && !messagePart->isSigned && !block.text().trimmed().isEmpty()) {
                mp->setText(aCodec->toUnicode(block.text()));
            }

            if (messagePart->isEncrypted) {
                mEncryptionState = KMMsgPartiallyEncrypted;
            }

            if (messagePart->isSigned) {
                mSignatureState = KMMsgPartiallySigned;
            }
        }

        //Do we have an fully Signed/Encrypted Message?
        if (fullySignedOrEncrypted) {
            if (mSignatureState == KMMsgPartiallySigned) {
                mSignatureState = KMMsgFullySigned;
            }
            if (mEncryptionState == KMMsgPartiallyEncrypted) {
                mEncryptionState = KMMsgFullyEncrypted;
            }
        }
    }
}

KMMsgEncryptionState TextMessagePart::encryptionState() const
{
    return mEncryptionState;
}

KMMsgSignatureState TextMessagePart::signatureState() const
{
    return mSignatureState;
}

bool TextMessagePart::showLink() const
{
    return !temporaryFilePath().isEmpty();
}

bool TextMessagePart::isFirstTextPart() const
{
    return content()->topLevel()->textContent() == content();
}

bool TextMessagePart::hasLabel() const
{
    return !NodeHelper::fileName(content()).isEmpty();
}

QString TextMessagePart::label() const
{
    const QString name = content()->contentType()->name();
    QString label = name.isEmpty() ? NodeHelper::fileName(content()) : name;
    if (label.isEmpty()) {
        label = i18nc("display name for an unnamed attachment", "Unnamed");
    }
    return label;
}

QString TextMessagePart::comment() const
{
    const QString comment = content()->contentDescription()->asUnicodeString();
    if (comment == label()) {
        return {};
    }
    return comment;
}

QString TextMessagePart::temporaryFilePath() const
{
    return nodeHelper()->writeNodeToTempFile(content());
}

//-----AttachmentMessageBlock----------------------

AttachmentMessagePart::AttachmentMessagePart(ObjectTreeParser *otp, KMime::Content *node, bool decryptMessage)
    : TextMessagePart(otp, node, decryptMessage)
{
}

AttachmentMessagePart::~AttachmentMessagePart()
{
}

//-----HtmlMessageBlock----------------------

HtmlMessagePart::HtmlMessagePart(ObjectTreeParser *otp, KMime::Content *node, Interface::ObjectTreeSource *source)
    : MessagePart(otp, QString())
    , mSource(source)
{
    if (!node) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }
    setContent(node);

    const QByteArray partBody(node->decodedContent());
    mBodyHTML = mOtp->codecFor(node)->toUnicode(partBody);
    mCharset = NodeHelper::charset(node);
}

HtmlMessagePart::~HtmlMessagePart()
{
}

void HtmlMessagePart::fix() const
{
    mOtp->mHtmlContent += mBodyHTML;
    mOtp->mHtmlContentCharset = mCharset;
}

QString HtmlMessagePart::text() const
{
    return mBodyHTML;
}

QString MimeTreeParser::HtmlMessagePart::plaintextContent() const
{
    return QString();
}

bool HtmlMessagePart::isHtml() const
{
    return true;
}

QString HtmlMessagePart::bodyHtml() const
{
    return mBodyHTML;
}

//-----MimeMessageBlock----------------------

MimeMessagePart::MimeMessagePart(ObjectTreeParser *otp, KMime::Content *node, bool onlyOneMimePart)
    : MessagePart(otp, QString())
    , mOnlyOneMimePart(onlyOneMimePart)
{
    if (!node) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }
    setContent(node);

    parseInternal(node, mOnlyOneMimePart);
}

MimeMessagePart::~MimeMessagePart()
{
}

QString MimeMessagePart::text() const
{
    return renderInternalText();
}

QString MimeMessagePart::plaintextContent() const
{
    return QString();
}

QString MimeMessagePart::htmlContent() const
{
    return QString();
}

//-----AlternativeMessagePart----------------------

AlternativeMessagePart::AlternativeMessagePart(ObjectTreeParser *otp, KMime::Content *node, Util::HtmlMode preferredMode)
    : MessagePart(otp, QString())
    , mPreferredMode(preferredMode)
{
    setContent(node);
    KMime::Content *dataIcal = findTypeInDirectChilds(node, "text/calendar");
    KMime::Content *dataHtml = findTypeInDirectChilds(node, "text/html");
    KMime::Content *dataText = findTypeInDirectChilds(node, "text/plain");

    if (!dataHtml) {
        // If we didn't find the HTML part as the first child of the multipart/alternative, it might
        // be that this is a HTML message with images, and text/plain and multipart/related are the
        // immediate children of this multipart/alternative node.
        // In this case, the HTML node is a child of multipart/related.
        dataHtml = findTypeInDirectChilds(node, "multipart/related");

        // Still not found? Stupid apple mail actually puts the attachments inside of the
        // multipart/alternative, which is wrong. Therefore we also have to look for multipart/mixed
        // here.
        // Do this only when preferring HTML mail, though, since otherwise the attachments are hidden
        // when displaying plain text.
        if (!dataHtml) {
            dataHtml = findTypeInDirectChilds(node, "multipart/mixed");
        }
    }

    if (dataIcal) {
        mChildNodes[Util::MultipartIcal] = dataIcal;
    }

    if (dataText) {
        mChildNodes[Util::MultipartPlain] = dataText;
    }

    if (dataHtml) {
        mChildNodes[Util::MultipartHtml] = dataHtml;
    }

    if (mChildNodes.isEmpty()) {
        qCWarning(MIMETREEPARSER_LOG) << "no valid nodes";
        return;
    }

    QMapIterator<Util::HtmlMode, KMime::Content *> i(mChildNodes);
    while (i.hasNext()) {
        i.next();
        mChildParts[i.key()] = MimeMessagePart::Ptr(new MimeMessagePart(mOtp, i.value(), true));
    }
}

AlternativeMessagePart::~AlternativeMessagePart()
{
}

Util::HtmlMode AlternativeMessagePart::preferredMode() const
{
    return mPreferredMode;
}

void AlternativeMessagePart::setPreferredMode(Util::HtmlMode preferredMode)
{
    mPreferredMode = preferredMode;
}

QList<Util::HtmlMode> AlternativeMessagePart::availableModes()
{
    return mChildParts.keys();
}

QString AlternativeMessagePart::text() const
{
    if (mChildParts.contains(Util::MultipartPlain)) {
        return mChildParts[Util::MultipartPlain]->text();
    }
    return QString();
}

void AlternativeMessagePart::fix() const
{
    if (mChildParts.contains(Util::MultipartPlain)) {
        mChildParts[Util::MultipartPlain]->fix();
    }

    const auto mode = preferredMode();
    if (mode != Util::MultipartPlain && mChildParts.contains(mode)) {
        mChildParts[mode]->fix();
    }
}

const QMap<Util::HtmlMode, MimeMessagePart::Ptr> &AlternativeMessagePart::childParts() const
{
    return mChildParts;
}

bool AlternativeMessagePart::isHtml() const
{
    return mChildParts.contains(Util::MultipartHtml);
}

QString AlternativeMessagePart::plaintextContent() const
{
    return text();
}

QString AlternativeMessagePart::htmlContent() const
{
    if (mChildParts.contains(Util::MultipartHtml)) {
        return mChildParts[Util::MultipartHtml]->text();
    } else {
        return plaintextContent();
    }
}

//-----CertMessageBlock----------------------

CertMessagePart::CertMessagePart(ObjectTreeParser *otp, KMime::Content *node, const QGpgME::Protocol *cryptoProto, bool autoImport)
    : MessagePart(otp, QString())
    , mAutoImport(autoImport)
    , mCryptoProto(cryptoProto)
{
    if (!node) {
        qCWarning(MIMETREEPARSER_LOG) << "not a valid node";
        return;
    }
    setContent(node);

    if (!mAutoImport) {
        return;
    }

    const QByteArray certData = node->decodedContent();

    QGpgME::ImportJob *import = mCryptoProto->importJob();
    QGpgMEJobExecutor executor;
    mImportResult = executor.exec(import, certData);
}

CertMessagePart::~CertMessagePart()
{
}

QString CertMessagePart::text() const
{
    return QString();
}

const GpgME::ImportResult &CertMessagePart::importResult() const
{
    return mImportResult;
}

//-----SignedMessageBlock---------------------
SignedMessagePart::SignedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node)
    : MessagePart(otp, text)
    , mCryptoProto(cryptoProto)
    , mFromAddress(fromAddress)
    , mMementoName("verification")
{
    setContent(node);
    partMetaData()->technicalProblem = (mCryptoProto == nullptr);
    partMetaData()->isSigned = true;
    partMetaData()->isGoodSignature = false;
    partMetaData()->keyTrust = GpgME::Signature::Unknown;
    partMetaData()->status = i18n("Wrong Crypto Plug-In.");
    partMetaData()->status_code = GPGME_SIG_STAT_NONE;
}

SignedMessagePart::~SignedMessagePart()
{
}

void SignedMessagePart::setIsSigned(bool isSigned)
{
    partMetaData()->isSigned = isSigned;
}

bool SignedMessagePart::isSigned() const
{
    return partMetaData()->isSigned;
}

QByteArray SignedMessagePart::mementoName() const
{
    return mMementoName;
}

void SignedMessagePart::setMementoName(const QByteArray &name)
{
    mMementoName = name;
}

bool SignedMessagePart::okVerify(const QByteArray &data, const QByteArray &signature, KMime::Content *textNode)
{
    NodeHelper *nodeHelper = mOtp->nodeHelper();

    partMetaData()->isSigned = false;
    partMetaData()->technicalProblem = (mCryptoProto == nullptr);
    partMetaData()->keyTrust = GpgME::Signature::Unknown;
    partMetaData()->status = i18n("Wrong Crypto Plug-In.");
    partMetaData()->status_code = GPGME_SIG_STAT_NONE;

    const QByteArray _mementoName = mementoName();

    CryptoBodyPartMemento *m = dynamic_cast<CryptoBodyPartMemento *>(nodeHelper->bodyPartMemento(content(), _mementoName));
    Q_ASSERT(!m || mCryptoProto); //No CryptoPlugin and having a bodyPartMemento -> there is something completely wrong

    if (!m && mCryptoProto) {
        if (!signature.isEmpty()) {
            QGpgME::VerifyDetachedJob *job = mCryptoProto->verifyDetachedJob();
            if (job) {
                m = new VerifyDetachedBodyPartMemento(job, mCryptoProto->keyListJob(), signature, data);
            }
        } else {
            QGpgME::VerifyOpaqueJob *job = mCryptoProto->verifyOpaqueJob();
            if (job) {
                m = new VerifyOpaqueBodyPartMemento(job, mCryptoProto->keyListJob(), data);
            }
        }
        if (m) {
            if (mOtp->allowAsync()) {
                QObject::connect(m, &CryptoBodyPartMemento::update,
                                 nodeHelper, &NodeHelper::update);
                if (m->start()) {
                    partMetaData()->inProgress = true;
                    mOtp->mHasPendingAsyncJobs = true;
                }
            } else {
                m->exec();
            }
            nodeHelper->setBodyPartMemento(content(), _mementoName, m);
        }
    } else if (m && m->isRunning()) {
        partMetaData()->inProgress = true;
        mOtp->mHasPendingAsyncJobs = true;
    } else {
        partMetaData()->inProgress = false;
        mOtp->mHasPendingAsyncJobs = false;
    }

    if (m && !partMetaData()->inProgress) {
        if (!signature.isEmpty()) {
            mVerifiedText = data;
        }
        setVerificationResult(m, textNode);
    }

    if (!m && !partMetaData()->inProgress) {
        QString errorMsg;
        QString cryptPlugLibName;
        QString cryptPlugDisplayName;
        if (mCryptoProto) {
            cryptPlugLibName = mCryptoProto->name();
            cryptPlugDisplayName = mCryptoProto->displayName();
        }

        if (!mCryptoProto) {
            if (cryptPlugDisplayName.isEmpty()) {
                errorMsg = i18n("No appropriate crypto plug-in was found.");
            } else {
                errorMsg = i18nc("%1 is either 'OpenPGP' or 'S/MIME'",
                                 "No %1 plug-in was found.",
                                 cryptPlugDisplayName);
            }
        } else {
            errorMsg = i18n("Crypto plug-in \"%1\" cannot verify signatures.",
                            cryptPlugLibName);
        }
        partMetaData()->errorText = i18n("The message is signed, but the "
                                         "validity of the signature cannot be "
                                         "verified.<br />"
                                         "Reason: %1",
                                         errorMsg);
    }

    return partMetaData()->isSigned;
}

static int signatureToStatus(const GpgME::Signature &sig)
{
    switch (sig.status().code()) {
    case GPG_ERR_NO_ERROR:
        return GPGME_SIG_STAT_GOOD;
    case GPG_ERR_BAD_SIGNATURE:
        return GPGME_SIG_STAT_BAD;
    case GPG_ERR_NO_PUBKEY:
        return GPGME_SIG_STAT_NOKEY;
    case GPG_ERR_NO_DATA:
        return GPGME_SIG_STAT_NOSIG;
    case GPG_ERR_SIG_EXPIRED:
        return GPGME_SIG_STAT_GOOD_EXP;
    case GPG_ERR_KEY_EXPIRED:
        return GPGME_SIG_STAT_GOOD_EXPKEY;
    default:
        return GPGME_SIG_STAT_ERROR;
    }
}

QString prettifyDN(const char *uid)
{
    return QGpgME::DN(uid).prettyDN();
}

void SignedMessagePart::sigStatusToMetaData()
{
    GpgME::Key key;
    if (partMetaData()->isSigned) {
        GpgME::Signature signature = mSignatures.front();
        partMetaData()->status_code = signatureToStatus(signature);
        partMetaData()->isGoodSignature = partMetaData()->status_code & GPGME_SIG_STAT_GOOD;
        // save extended signature status flags
        partMetaData()->sigSummary = signature.summary();

        if (partMetaData()->isGoodSignature && !key.keyID()) {
            // Search for the key by its fingerprint so that we can check for
            // trust etc.
            QGpgME::KeyListJob *job = mCryptoProto->keyListJob(false, false, false);    // local, no sigs
            if (!job) {
                qCDebug(MIMETREEPARSER_LOG) << "The Crypto backend does not support listing keys. ";
            } else {
                std::vector<GpgME::Key> found_keys;
                // As we are local it is ok to make this synchronous
                GpgME::KeyListResult res = job->exec(QStringList(QLatin1String(signature.fingerprint())), false, found_keys);
                if (res.error()) {
                    qCDebug(MIMETREEPARSER_LOG) << "Error while searching key for Fingerprint: " << signature.fingerprint();
                }
                if (found_keys.size() > 1) {
                    // Should not Happen
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found more then one Key for Fingerprint: " << signature.fingerprint();
                }
                if (found_keys.size() != 1) {
                    // Should not Happen at this point
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found no Key for Fingerprint: " << signature.fingerprint();
                } else {
                    key = found_keys[0];
                }
                delete job;
            }
        }

        if (key.keyID()) {
            partMetaData()->keyId = key.keyID();
        }
        if (partMetaData()->keyId.isEmpty()) {
            partMetaData()->keyId = signature.fingerprint();
        }
        partMetaData()->keyTrust = signature.validity();
        if (key.numUserIDs() > 0 && key.userID(0).id()) {
            partMetaData()->signer = prettifyDN(key.userID(0).id());
        }
        for (uint iMail = 0; iMail < key.numUserIDs(); ++iMail) {
            // The following if /should/ always result in TRUE but we
            // won't trust implicitly the plugin that gave us these data.
            if (key.userID(iMail).email()) {
                QString email = QString::fromUtf8(key.userID(iMail).email());
                // ### work around gpgme 0.3.QString text() const override;x / cryptplug bug where the
                // ### email addresses are specified as angle-addr, not addr-spec:
                if (email.startsWith(QLatin1Char('<')) && email.endsWith(QLatin1Char('>'))) {
                    email = email.mid(1, email.length() - 2);
                }
                if (!email.isEmpty()) {
                    partMetaData()->signerMailAddresses.append(email);
                }
            }
        }

        if (signature.creationTime()) {
            partMetaData()->creationTime.setSecsSinceEpoch(signature.creationTime());
        } else {
            partMetaData()->creationTime = QDateTime();
        }
        if (partMetaData()->signer.isEmpty()) {
            if (key.numUserIDs() > 0 && key.userID(0).name()) {
                partMetaData()->signer = prettifyDN(key.userID(0).name());
            }
            if (!partMetaData()->signerMailAddresses.empty()) {
                if (partMetaData()->signer.isEmpty()) {
                    partMetaData()->signer = partMetaData()->signerMailAddresses.front();
                } else {
                    partMetaData()->signer += QLatin1String(" <") + partMetaData()->signerMailAddresses.front() + QLatin1Char('>');
                }
            }
        }
    }
}

void SignedMessagePart::startVerification(const QByteArray &text, const QTextCodec *aCodec)
{
    startVerificationDetached(text, nullptr, QByteArray());

    if (!content() && partMetaData()->isSigned) {
        setText(aCodec->toUnicode(mVerifiedText));
    }
}

void SignedMessagePart::startVerificationDetached(const QByteArray &text, KMime::Content *textNode, const QByteArray &signature)
{
    partMetaData()->isEncrypted = false;
    partMetaData()->isDecryptable = false;

    if (textNode) {
        parseInternal(textNode, false);
    }

    okVerify(text, signature, textNode);

    if (!partMetaData()->isSigned) {
        partMetaData()->creationTime = QDateTime();
    }
}

void SignedMessagePart::setVerificationResult(const CryptoBodyPartMemento *m, KMime::Content *textNode)
{
    {
        const auto vm = dynamic_cast<const VerifyDetachedBodyPartMemento *>(m);
        if (vm) {
            mSignatures = vm->verifyResult().signatures();
        }
    }
    {
        const auto vm = dynamic_cast<const VerifyOpaqueBodyPartMemento *>(m);
        if (vm) {
            mVerifiedText = vm->plainText();
            mSignatures = vm->verifyResult().signatures();
        }
    }
    {
        const auto vm = dynamic_cast<const DecryptVerifyBodyPartMemento *>(m);
        if (vm) {
            mVerifiedText = vm->plainText();
            mSignatures = vm->verifyResult().signatures();
        }
    }
    partMetaData()->auditLogError = m->auditLogError();
    partMetaData()->auditLog = m->auditLogAsHtml();
    partMetaData()->isSigned = !mSignatures.empty();

    if (partMetaData()->isSigned) {
        sigStatusToMetaData();
        if (content()) {
            mOtp->nodeHelper()->setSignatureState(content(), KMMsgFullySigned);
            if (!textNode) {
                mOtp->nodeHelper()->setPartMetaData(content(), *partMetaData());

                if (!mVerifiedText.isEmpty()) {
                    auto tempNode = new KMime::Content();
                    tempNode->setContent(KMime::CRLFtoLF(mVerifiedText.constData()));
                    tempNode->parse();

                    if (!tempNode->head().isEmpty()) {
                        tempNode->contentDescription()->from7BitString("signed data");
                    }
                    mOtp->nodeHelper()->attachExtraContent(content(), tempNode);

                    parseInternal(tempNode, false);
                }
            }
        }
    }
}

QString SignedMessagePart::plaintextContent() const
{
    if (!content()) {
        return MessagePart::text();
    } else {
        return QString();
    }
}

QString SignedMessagePart::htmlContent() const
{
    if (!content()) {
        return MessagePart::text();
    } else {
        return QString();
    }
}

const QGpgME::Protocol *SignedMessagePart::cryptoProto() const
{
    return mCryptoProto;
}

QString SignedMessagePart::fromAddress() const
{
    return mFromAddress;
}

bool SignedMessagePart::hasHeader(const char *header) const
{
    const auto extraContent = mOtp->nodeHelper()->decryptedNodeForContent(content());
    if (extraContent) {
        return extraContent->hasHeader(header);
    }
    return false;
}

KMime::Headers::Base *SignedMessagePart::header(const char *header) const
{
    const auto extraContent = mOtp->nodeHelper()->decryptedNodeForContent(content());
    if (extraContent) {
        return extraContent->headerByType(header);
    }
    return nullptr;
}

//-----CryptMessageBlock---------------------
EncryptedMessagePart::EncryptedMessagePart(ObjectTreeParser *otp, const QString &text, const QGpgME::Protocol *cryptoProto, const QString &fromAddress, KMime::Content *node)
    : MessagePart(otp, text)
    , mPassphraseError(false)
    , mNoSecKey(false)
    , mDecryptMessage(false)
    , mCryptoProto(cryptoProto)
    , mFromAddress(fromAddress)
    , mMementoName("decryptverify")
{
    setContent(node);
    partMetaData()->technicalProblem = (mCryptoProto == nullptr);
    partMetaData()->isSigned = false;
    partMetaData()->isGoodSignature = false;
    partMetaData()->isEncrypted = false;
    partMetaData()->isDecryptable = false;
    partMetaData()->keyTrust = GpgME::Signature::Unknown;
    partMetaData()->status = i18n("Wrong Crypto Plug-In.");
    partMetaData()->status_code = GPGME_SIG_STAT_NONE;
}

EncryptedMessagePart::~EncryptedMessagePart()
{
}

void EncryptedMessagePart::setDecryptMessage(bool decrypt)
{
    mDecryptMessage = decrypt;
}

bool EncryptedMessagePart::decryptMessage() const
{
    return mDecryptMessage;
}

void EncryptedMessagePart::setIsEncrypted(bool encrypted)
{
    partMetaData()->isEncrypted = encrypted;
}

bool EncryptedMessagePart::isEncrypted() const
{
    return partMetaData()->isEncrypted;
}

bool EncryptedMessagePart::isDecryptable() const
{
    return partMetaData()->isDecryptable;
}

bool EncryptedMessagePart::isNoSecKey() const
{
    return mNoSecKey;
}

bool EncryptedMessagePart::passphraseError() const
{
    return mPassphraseError;
}

QByteArray EncryptedMessagePart::mementoName() const
{
    return mMementoName;
}

void EncryptedMessagePart::setMementoName(const QByteArray &name)
{
    mMementoName = name;
}

void EncryptedMessagePart::startDecryption(const QByteArray &text, const QTextCodec *aCodec)
{
    KMime::Content *content = new KMime::Content;
    content->setBody(text);
    content->parse();

    startDecryption(content);

    if (!partMetaData()->inProgress && partMetaData()->isDecryptable) {
        if (hasSubParts()) {
            auto _mp = (subParts()[0]).dynamicCast<SignedMessagePart>();
            if (_mp) {
                _mp->setText(aCodec->toUnicode(mDecryptedData));
            } else {
                setText(aCodec->toUnicode(mDecryptedData));
            }
        } else {
            setText(aCodec->toUnicode(mDecryptedData));
        }
    }
}

bool EncryptedMessagePart::okDecryptMIME(KMime::Content &data)
{
    mPassphraseError = false;
    partMetaData()->inProgress = false;
    partMetaData()->errorText.clear();
    partMetaData()->auditLogError = GpgME::Error();
    partMetaData()->auditLog.clear();
    bool bDecryptionOk = false;
    bool cannotDecrypt = false;
    NodeHelper *nodeHelper = mOtp->nodeHelper();

    Q_ASSERT(decryptMessage());

    const QByteArray _mementoName = mementoName();
    // Check whether the memento contains a result from last time:
    const DecryptVerifyBodyPartMemento *m
        = dynamic_cast<DecryptVerifyBodyPartMemento *>(nodeHelper->bodyPartMemento(&data, _mementoName));

    Q_ASSERT(!m || mCryptoProto); //No CryptoPlugin and having a bodyPartMemento -> there is something completely wrong

    if (!m && mCryptoProto) {
        QGpgME::DecryptVerifyJob *job = mCryptoProto->decryptVerifyJob();
        if (!job) {
            cannotDecrypt = true;
        } else {
            const QByteArray ciphertext = data.decodedContent();
            DecryptVerifyBodyPartMemento *newM
                = new DecryptVerifyBodyPartMemento(job, ciphertext);
            if (mOtp->allowAsync()) {
                QObject::connect(newM, &CryptoBodyPartMemento::update,
                                 nodeHelper, &NodeHelper::update);
                if (newM->start()) {
                    partMetaData()->inProgress = true;
                    mOtp->mHasPendingAsyncJobs = true;
                } else {
                    m = newM;
                }
            } else {
                newM->exec();
                m = newM;
            }
            nodeHelper->setBodyPartMemento(&data, _mementoName, newM);
        }
    } else if (m && m->isRunning()) {
        partMetaData()->inProgress = true;
        mOtp->mHasPendingAsyncJobs = true;
        m = nullptr;
    }

    if (m) {
        const QByteArray &plainText = m->plainText();
        const GpgME::DecryptionResult &decryptResult = m->decryptResult();
        const GpgME::VerificationResult &verifyResult = m->verifyResult();
        partMetaData()->isSigned = verifyResult.signatures().size() > 0;

        if (partMetaData()->isSigned) {
            auto subPart = SignedMessagePart::Ptr(new SignedMessagePart(mOtp, MessagePart::text(), mCryptoProto, mFromAddress, content()));
            subPart->setVerificationResult(m, nullptr);
            appendSubPart(subPart);
        }

        mDecryptRecipients.clear();
        bDecryptionOk = !decryptResult.error();

//         std::stringstream ss;
//         ss << decryptResult << '\n' << verifyResult;
//         qCDebug(MIMETREEPARSER_LOG) << ss.str().c_str();

        for (const auto &recipient : decryptResult.recipients()) {
            if (!recipient.status()) {
                bDecryptionOk = true;
            }
            GpgME::Key key;
            QGpgME::KeyListJob *job = mCryptoProto->keyListJob(false, false, false);    // local, no sigs
            if (!job) {
                qCDebug(MIMETREEPARSER_LOG) << "The Crypto backend does not support listing keys. ";
            } else {
                std::vector<GpgME::Key> found_keys;
                // As we are local it is ok to make this synchronous
                GpgME::KeyListResult res = job->exec(QStringList(QLatin1String(recipient.keyID())), false, found_keys);
                if (res.error()) {
                    qCDebug(MIMETREEPARSER_LOG) << "Error while searching key for Fingerprint: " << recipient.keyID();
                }
                if (found_keys.size() > 1) {
                    // Should not Happen
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found more then one Key for Fingerprint: " << recipient.keyID();
                }
                if (found_keys.size() != 1) {
                    // Should not Happen at this point
                    qCDebug(MIMETREEPARSER_LOG) << "Oops: Found no Key for Fingerprint: " << recipient.keyID();
                } else {
                    key = found_keys[0];
                }
            }
            mDecryptRecipients.push_back(std::make_pair(recipient, key));
        }

        if (!bDecryptionOk && partMetaData()->isSigned) {
            //Only a signed part
            partMetaData()->isEncrypted = false;
            bDecryptionOk = true;
            mDecryptedData = plainText;
        } else {
            mPassphraseError = decryptResult.error().isCanceled() || decryptResult.error().code() == GPG_ERR_NO_SECKEY;
            partMetaData()->isEncrypted = bDecryptionOk || decryptResult.error().code() != GPG_ERR_NO_DATA;

            if (decryptResult.error().isCanceled()) {
                setDecryptMessage(false);
            }

            partMetaData()->errorText = QString::fromLocal8Bit(decryptResult.error().asString());
            if (partMetaData()->isEncrypted && decryptResult.numRecipients() > 0) {
                partMetaData()->keyId = decryptResult.recipient(0).keyID();
            }

            if (bDecryptionOk) {
                mDecryptedData = plainText;
            } else {
                mNoSecKey = true;
                foreach (const GpgME::DecryptionResult::Recipient &recipient, decryptResult.recipients()) {
                    mNoSecKey &= (recipient.status().code() == GPG_ERR_NO_SECKEY);
                }
                if (!mPassphraseError && !mNoSecKey) {          // GpgME do not detect passphrase error correctly
                    mPassphraseError = true;
                }
            }
        }
    }

    if (!bDecryptionOk) {
        QString cryptPlugLibName;
        if (mCryptoProto) {
            cryptPlugLibName = mCryptoProto->name();
        }

        if (!mCryptoProto) {
            partMetaData()->errorText = i18n("No appropriate crypto plug-in was found.");
        } else if (cannotDecrypt) {
            partMetaData()->errorText = i18n("Crypto plug-in \"%1\" cannot decrypt messages.",
                                             cryptPlugLibName);
        } else if (!passphraseError()) {
            partMetaData()->errorText = i18n("Crypto plug-in \"%1\" could not decrypt the data.", cryptPlugLibName)
                                        + QLatin1String("<br />")
                                        + i18n("Error: %1", partMetaData()->errorText);
        }
    }
    return bDecryptionOk;
}

void EncryptedMessagePart::startDecryption(KMime::Content *data)
{
    if (!content() && !data) {
        return;
    }

    if (!data) {
        data = content();
    }

    partMetaData()->isEncrypted = true;

    bool bOkDecrypt = okDecryptMIME(*data);

    if (partMetaData()->inProgress) {
        return;
    }
    partMetaData()->isDecryptable = bOkDecrypt;

    if (!partMetaData()->isDecryptable) {
        setText(QString::fromUtf8(mDecryptedData.constData()));
    }

    if (partMetaData()->isEncrypted && !decryptMessage()) {
        partMetaData()->isDecryptable = true;
    }

    if (content() && !partMetaData()->isSigned) {
        mOtp->nodeHelper()->setPartMetaData(content(), *partMetaData());

        if (decryptMessage()) {
            auto tempNode = new KMime::Content();
            tempNode->setContent(KMime::CRLFtoLF(mDecryptedData.constData()));
            tempNode->parse();

            if (!tempNode->head().isEmpty()) {
                tempNode->contentDescription()->from7BitString("encrypted data");
            }
            mOtp->nodeHelper()->attachExtraContent(content(), tempNode);

            parseInternal(tempNode, false);
        }
    }
}

QString EncryptedMessagePart::plaintextContent() const
{
    if (!content()) {
        return MessagePart::text();
    } else {
        return QString();
    }
}

QString EncryptedMessagePart::htmlContent() const
{
    if (!content()) {
        return MessagePart::text();
    } else {
        return QString();
    }
}

QString EncryptedMessagePart::text() const
{
    if (hasSubParts()) {
        auto _mp = (subParts()[0]).dynamicCast<SignedMessagePart>();
        if (_mp) {
            return _mp->text();
        } else {
            return MessagePart::text();
        }
    } else {
        return MessagePart::text();
    }
}

const QGpgME::Protocol *EncryptedMessagePart::cryptoProto() const
{
    return mCryptoProto;
}

QString EncryptedMessagePart::fromAddress() const
{
    return mFromAddress;
}

const std::vector<std::pair<GpgME::DecryptionResult::Recipient, GpgME::Key> > &EncryptedMessagePart::decryptRecipients() const
{
    return mDecryptRecipients;
}

bool EncryptedMessagePart::hasHeader(const char *header) const
{
    const auto extraContent = mOtp->nodeHelper()->decryptedNodeForContent(content());
    if (extraContent) {
        return extraContent->hasHeader(header);
    }
    return false;
}

KMime::Headers::Base *EncryptedMessagePart::header(const char *header) const
{
    const auto extraContent = mOtp->nodeHelper()->decryptedNodeForContent(content());
    if (extraContent) {
        return extraContent->headerByType(header);
    }
    return nullptr;
}

EncapsulatedRfc822MessagePart::EncapsulatedRfc822MessagePart(ObjectTreeParser *otp, KMime::Content *node, const KMime::Message::Ptr &message)
    : MessagePart(otp, QString())
    , mMessage(message)
{
    setContent(node);
    partMetaData()->isEncrypted = false;
    partMetaData()->isSigned = false;
    partMetaData()->isEncapsulatedRfc822Message = true;

    mOtp->nodeHelper()->setNodeDisplayedEmbedded(node, true);
    mOtp->nodeHelper()->setPartMetaData(node, *partMetaData());

    if (!mMessage) {
        qCWarning(MIMETREEPARSER_LOG) << "Node is of type message/rfc822 but doesn't have a message!";
        return;
    }

    // The link to "Encapsulated message" is clickable, therefore the temp file needs to exists,
    // since the user can click the link and expect to have normal attachment operations there.
    mOtp->nodeHelper()->writeNodeToTempFile(message.data());

    parseInternal(message.data(), false);
}

EncapsulatedRfc822MessagePart::~EncapsulatedRfc822MessagePart()
{
}

QString EncapsulatedRfc822MessagePart::text() const
{
    return renderInternalText();
}

void EncapsulatedRfc822MessagePart::fix() const
{
}

const KMime::Message::Ptr EncapsulatedRfc822MessagePart::message() const
{
    return mMessage;
}
