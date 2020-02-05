/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimchecksignaturejob.h"
#include "dkimdownloadkeyjob.h"
#include "dkimmanagerkey.h"
#include "dkiminfo.h"
#include "dkimutil.h"
#include "dkimkeyrecord.h"
#include "messageviewer_dkimcheckerdebug.h"

#include <KEmailAddress>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFile>
#include <qca_publickey.h>

//see https://tools.ietf.org/html/rfc6376
//#define DEBUG_SIGNATURE_DKIM 1
using namespace MessageViewer;
DKIMCheckSignatureJob::DKIMCheckSignatureJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckSignatureJob::~DKIMCheckSignatureJob()
{
}

MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult DKIMCheckSignatureJob::createCheckResult()
{
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult result;
    result.error = mError;
    result.warning = mWarning;
    result.status = mStatus;
    result.sdid = mDkimInfo.domain();
    result.auid = mDkimInfo.agentOrUserIdentifier();
    result.fromEmail = mFromEmail;
    result.listSignatureAuthenticationResult = mCheckSignatureAuthenticationResult;
    return result;
}

QString DKIMCheckSignatureJob::bodyCanonizationResult() const
{
    return mBodyCanonizationResult;
}

QString DKIMCheckSignatureJob::headerCanonizationResult() const
{
    return mHeaderCanonizationResult;
}

void DKIMCheckSignatureJob::start()
{
    if (!mMessage) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Item has not a message";
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    if (auto hrd = mMessage->headerByType("DKIM-Signature")) {
        mDkimValue = hrd->asUnicodeString();
    }
    //Store mFromEmail before looking at mDkimValue value. Otherwise we can return a from empty
    if (auto hrd = mMessage->from(false)) {
        mFromEmail = KEmailAddress::extractEmailAddress(hrd->asUnicodeString());
    }
    if (mDkimValue.isEmpty()) {
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "mFromEmail " << mFromEmail;
    if (!mDkimInfo.parseDKIM(mDkimValue)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to parse header" << mDkimValue;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    const MessageViewer::DKIMCheckSignatureJob::DKIMStatus status = checkSignature(mDkimInfo);
    if (status != MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid) {
        mStatus = status;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    //ComputeBodyHash now.
    switch (mDkimInfo.bodyCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyCanonicalization;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        mBodyCanonizationResult = bodyCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        mBodyCanonizationResult = bodyCanonizationRelaxed();
        break;
    }
    //qDebug() << " bodyCanonizationResult "<< mBodyCanonizationResult << " algorithm " << mDkimInfo.hashingAlgorithm() << mDkimInfo.bodyHash();

    if (mDkimInfo.bodyLengthCount() != -1) { //Verify it.
        if (mDkimInfo.bodyLengthCount() > mBodyCanonizationResult.length()) {
            // length tag exceeds body size
            qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << " mDkimInfo.bodyLengthCount() " << mDkimInfo.bodyLengthCount() << " mBodyCanonizationResult.length() " << mBodyCanonizationResult.length();
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::SignatureTooLarge;
            mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
            Q_EMIT result(createCheckResult());
            deleteLater();
            return;
        } else if (mDkimInfo.bodyLengthCount() < mBodyCanonizationResult.length()) {
            mWarning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::SignatureTooSmall;
        }
        // truncated body to the length specified in the "l=" tag
        mBodyCanonizationResult = mBodyCanonizationResult.left(mDkimInfo.bodyLengthCount());
    }
    if (mBodyCanonizationResult.startsWith(QLatin1String("\r\n"))) { //Remove it from start
        mBodyCanonizationResult = mBodyCanonizationResult.right(mBodyCanonizationResult.length() -2);
    }
#ifdef DEBUG_SIGNATURE_DKIM
    QFile caFile(QStringLiteral("/tmp/bodycanon-kmail.txt"));
    caFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outStream(&caFile);
    outStream << mBodyCanonizationResult;
    caFile.close();
#endif

    QByteArray resultHash;
    switch (mDkimInfo.hashingAlgorithm()) {
    case DKIMInfo::HashingAlgorithmType::Sha1:
        resultHash = MessageViewer::DKIMUtil::generateHash(mBodyCanonizationResult.toLatin1(), QCryptographicHash::Sha1);
        break;
    case DKIMInfo::HashingAlgorithmType::Sha256:
        resultHash = MessageViewer::DKIMUtil::generateHash(mBodyCanonizationResult.toLatin1(), QCryptographicHash::Sha256);
        break;
    case DKIMInfo::HashingAlgorithmType::Any:
    case DKIMInfo::HashingAlgorithmType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InsupportedHashAlgorithm;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    // compare body hash
    qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "resultHash "  << resultHash << "mDkimInfo.bodyHash()" << mDkimInfo.bodyHash();
    if (resultHash != mDkimInfo.bodyHash().toLatin1()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << " Corrupted body hash";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::CorruptedBodyHash;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    if (mDkimInfo.headerCanonization() == MessageViewer::DKIMInfo::CanonicalizationType::Unknown) {
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidHeaderCanonicalization;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    //Parse message header
    if (!mHeaderParser.wasAlreadyParsed()) {
        mHeaderParser.setHead(mMessage->head());
        mHeaderParser.parse();
    }

    computeHeaderCanonization(true);
    if (mPolicy.saveKey() == MessageViewer::MessageViewerSettings::EnumSaveKey::Save) {
        const QString keyValue = MessageViewer::DKIMManagerKey::self()->keyValue(mDkimInfo.selector(), mDkimInfo.domain());
        //qDebug() << " mDkimInfo.selector() " << mDkimInfo.selector() << "mDkimInfo.domain()  " << mDkimInfo.domain() << keyValue;
        if (keyValue.isEmpty()) {
            downloadKey(mDkimInfo);
        } else {
            parseDKIMKeyRecord(keyValue, mDkimInfo.domain(), mDkimInfo.selector(), false);
        }
    } else {
        downloadKey(mDkimInfo);
    }
}

void DKIMCheckSignatureJob::computeHeaderCanonization(bool removeQuoteOnContentType)
{
    //Compute Hash Header
    switch (mDkimInfo.headerCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        mHeaderCanonizationResult = headerCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        mHeaderCanonizationResult = headerCanonizationRelaxed(removeQuoteOnContentType);
        break;
    }

    //    In hash step 2, the Signer/Verifier MUST pass the following to the
    //       hash algorithm in the indicated order.

    //       1.  The header fields specified by the "h=" tag, in the order
    //           specified in that tag, and canonicalized using the header
    //           canonicalization algorithm specified in the "c=" tag.  Each
    //           header field MUST be terminated with a single CRLF.

    //       2.  The DKIM-Signature header field that exists (verifying) or will
    //           be inserted (signing) in the message, with the value of the "b="
    //           tag (including all surrounding whitespace) deleted (i.e., treated
    //           as the empty string), canonicalized using the header
    //           canonicalization algorithm specified in the "c=" tag, and without
    //           a trailing CRLF.
    // add DKIM-Signature header to the hash input
    // with the value of the "b=" tag (including all surrounding whitespace) deleted

    //Add dkim-signature as lowercase

    QString dkimValue = mDkimValue;
    dkimValue = dkimValue.left(dkimValue.indexOf(QLatin1String("b=")) + 2);
    switch (mDkimInfo.headerCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        mHeaderCanonizationResult += QLatin1String("\r\n") + MessageViewer::DKIMUtil::headerCanonizationSimple(QLatin1String("dkim-signature"), dkimValue);
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        mHeaderCanonizationResult += QLatin1String("\r\n") + MessageViewer::DKIMUtil::headerCanonizationRelaxed(QLatin1String("dkim-signature"), dkimValue, removeQuoteOnContentType);
        break;
    }
#ifdef DEBUG_SIGNATURE_DKIM
    QFile headerFile(QStringLiteral("/tmp/headercanon-kmail-%1.txt").arg(removeQuoteOnContentType ? QLatin1String("removequote") : QLatin1String("withquote")));
    headerFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outHeaderStream(&headerFile);
    outHeaderStream << mHeaderCanonizationResult;
    headerFile.close();
#endif
}

void DKIMCheckSignatureJob::setHeaderParser(const DKIMHeaderParser &headerParser)
{
    mHeaderParser = headerParser;
}

void DKIMCheckSignatureJob::setCheckSignatureAuthenticationResult(const QVector<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst)
{
    mCheckSignatureAuthenticationResult = lst;
}

QString DKIMCheckSignatureJob::bodyCanonizationSimple() const
{
    /*
     * canonicalize the body using the simple algorithm
     * specified in Section 3.4.3 of RFC 6376
     */
    //    The "simple" body canonicalization algorithm ignores all empty lines
    //       at the end of the message body.  An empty line is a line of zero
    //       length after removal of the line terminator.  If there is no body or
    //       no trailing CRLF on the message body, a CRLF is added.  It makes no
    //       other changes to the message body.  In more formal terms, the
    //       "simple" body canonicalization algorithm converts "*CRLF" at the end
    //       of the body to a single "CRLF".

    //       Note that a completely empty or missing body is canonicalized as a
    //       single "CRLF"; that is, the canonicalized length will be 2 octets.

    return MessageViewer::DKIMUtil::bodyCanonizationSimple(QString::fromLatin1(mMessage->encodedBody()));
}

QString DKIMCheckSignatureJob::bodyCanonizationRelaxed() const
{
    /*
     * canonicalize the body using the relaxed algorithm
     * specified in Section 3.4.4 of RFC 6376
     */
    /*
        a.  Reduce whitespace:

            *  Ignore all whitespace at the end of lines.  Implementations
                MUST NOT remove the CRLF at the end of the line.

            *  Reduce all sequences of WSP within a line to a single SP
                character.

        b.  Ignore all empty lines at the end of the message body.  "Empty
            line" is defined in Section 3.4.3.  If the body is non-empty but
            does not end with a CRLF, a CRLF is added.  (For email, this is
            only possible when using extensions to SMTP or non-SMTP transport
            mechanisms.)
        */
    const QString returnValue = MessageViewer::DKIMUtil::bodyCanonizationRelaxed(QString::fromLatin1(mMessage->encodedBody()));
    return returnValue;
}

QString DKIMCheckSignatureJob::headerCanonizationSimple() const
{
    QString headers;

    DKIMHeaderParser parser = mHeaderParser;

    for (const QString &header : mDkimInfo.listSignedHeader()) {
        const QString str = parser.headerType(header.toLower());
        if (!str.isEmpty()) {
            if (!headers.isEmpty()) {
                headers += QLatin1String("\r\n");
            }
            headers += MessageViewer::DKIMUtil::headerCanonizationSimple(header, str);
        }
    }
    return headers;
}

QString DKIMCheckSignatureJob::headerCanonizationRelaxed(bool removeQuoteOnContentType) const
{
//    The "relaxed" header canonicalization algorithm MUST apply the
//       following steps in order:

//       o  Convert all header field names (not the header field values) to
//          lowercase.  For example, convert "SUBJect: AbC" to "subject: AbC".

//       o  Unfold all header field continuation lines as described in
//          [RFC5322]; in particular, lines with terminators embedded in
//          continued header field values (that is, CRLF sequences followed by
//          WSP) MUST be interpreted without the CRLF.  Implementations MUST
//          NOT remove the CRLF at the end of the header field value.

//       o  Convert all sequences of one or more WSP characters to a single SP
//          character.  WSP characters here include those before and after a
//          line folding boundary.

//       o  Delete all WSP characters at the end of each unfolded header field
//          value.

//       o  Delete any WSP characters remaining before and after the colon
//          separating the header field name from the header field value.  The
//          colon separator MUST be retained.

    QString headers;
    DKIMHeaderParser parser = mHeaderParser;
    for (const QString &header : mDkimInfo.listSignedHeader()) {
        const QString str = parser.headerType(header.toLower());
        if (!str.isEmpty()) {
            if (!headers.isEmpty()) {
                headers += QLatin1String("\r\n");
            }
            headers += MessageViewer::DKIMUtil::headerCanonizationRelaxed(header, str, removeQuoteOnContentType);
        }
    }
    return headers;
}

void DKIMCheckSignatureJob::downloadKey(const DKIMInfo &info)
{
    DKIMDownloadKeyJob *job = new DKIMDownloadKeyJob(this);
    job->setDomainName(info.domain());
    job->setSelectorName(info.selector());
    connect(job, &DKIMDownloadKeyJob::error, this, [this](const QString &errorString) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start downloadkey: error returned: " << errorString;
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::ImpossibleToDownloadKey;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
    });
    connect(job, &DKIMDownloadKeyJob::success, this, &DKIMCheckSignatureJob::slotDownloadKeyDone);

    if (!job->start()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start downloadkey";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::ImpossibleToDownloadKey;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
    }
}

void DKIMCheckSignatureJob::slotDownloadKeyDone(const QList<QByteArray> &lst, const QString &domain, const QString &selector)
{
    QByteArray ba;
    if (lst.count() != 1) {
        for (const QByteArray &b : lst) {
            ba += b;
        }
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Key result has more that 1 element" << lst;
    } else {
        ba = lst.at(0);
    }
    parseDKIMKeyRecord(QString::fromLocal8Bit(ba), domain, selector, true);
}

void DKIMCheckSignatureJob::parseDKIMKeyRecord(const QString &str, const QString &domain, const QString &selector, bool storeKeyValue)
{
    qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "void DKIMCheckSignatureJob::parseDKIMKeyRecord(const QString &str, const QString &domain, const QString &selector, bool storeKeyValue) key:" << str;
    if (!mDkimKeyRecord.parseKey(str)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to parse key record " << str;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    if (mDkimKeyRecord.keyType() != QLatin1String("rsa")) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "mDkimKeyRecord key type is unknown " << mDkimKeyRecord.keyType() << " str " << str;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    // if s flag is set in DKIM key record
    // AUID must be from the same domain as SDID (and not a subdomain)
    if (mDkimKeyRecord.flags().contains(QLatin1String("s"))) {
        //                  s  Any DKIM-Signature header fields using the "i=" tag MUST have
        //                     the same domain value on the right-hand side of the "@" in the
        //                     "i=" tag and the value of the "d=" tag.  That is, the "i="
        //                     domain MUST NOT be a subdomain of "d=".  Use of this flag is
        //                     RECOMMENDED unless subdomaining is required.
        if (mDkimInfo.iDomain() != mDkimInfo.domain()) {
            mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::DomainI;
            Q_EMIT result(createCheckResult());
            deleteLater();
            return;
        }
    }
    // check that the testing flag is not set
    if (mDkimKeyRecord.flags().contains(QLatin1String("y"))) {
        if (!mPolicy.verifySignatureWhenOnlyTest()) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Testing mode!";
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::TestKeyMode;
            mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
            Q_EMIT result(createCheckResult());
            deleteLater();
            return;
        }
    }
    if (mDkimKeyRecord.publicKey().isEmpty()) {
        // empty value means that this public key has been revoked
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "mDkimKeyRecord public key is empty. It was revoked ";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::PublicKeyWasRevoked;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    if (storeKeyValue) {
        Q_EMIT storeKey(str, domain, selector);
    }

    verifyRSASignature();
}

void DKIMCheckSignatureJob::verifyRSASignature()
{
    QCA::ConvertResult conversionResult;
    //qDebug() << "mDkimKeyRecord.publicKey() " <<mDkimKeyRecord.publicKey() << " QCA::base64ToArray(mDkimKeyRecord.publicKey() " <<QCA::base64ToArray(mDkimKeyRecord.publicKey());
    QCA::PublicKey publicKey = QCA::RSAPublicKey::fromDER(QCA::base64ToArray(mDkimKeyRecord.publicKey()), &conversionResult);
    if (QCA::ConvertGood != conversionResult) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Public key read failed" << conversionResult << " public key" <<mDkimKeyRecord.publicKey();
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::PublicKeyConversionError;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    } else {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Success loading public key";
    }
    QCA::RSAPublicKey rsaPublicKey = publicKey.toRSA();
    //qDebug() << "publicKey.modulus" << rsaPublicKey.n().toString();
    //qDebug() << "publicKey.exponent" << rsaPublicKey.e().toString();

    if (rsaPublicKey.e().toString().toLong() * 4 < 1024) {
        const int publicRsaTooSmallPolicyValue = mPolicy.publicRsaTooSmallPolicy();
        if (publicRsaTooSmallPolicyValue == MessageViewer::MessageViewerSettings::EnumPublicRsaTooSmall::Nothing) {
            ; //Nothing
        } else if (publicRsaTooSmallPolicyValue == MessageViewer::MessageViewerSettings::EnumPublicRsaTooSmall::Warning) {
            mWarning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::PublicRsaKeyTooSmall;
        } else if (publicRsaTooSmallPolicyValue == MessageViewer::MessageViewerSettings::EnumPublicRsaTooSmall::Error) {
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::PublicKeyTooSmall;
            mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
            Q_EMIT result(createCheckResult());
            deleteLater();
            return;
        }


    } else if (rsaPublicKey.e().toString().toLong() * 4 < 2048) {
        //TODO
    }
    //qDebug() << "mHeaderCanonizationResult " << mHeaderCanonizationResult << " mDkimInfo.signature() " << mDkimInfo.signature();
    if (rsaPublicKey.canVerify()) {
        const QString s = mDkimInfo.signature().remove(QLatin1Char(' '));
        QCA::SecureArray sec = mHeaderCanonizationResult.toLatin1();
        const QByteArray ba = QCA::base64ToArray(s);
        //qDebug() << " s base ba" << ba;
        QCA::SignatureAlgorithm sigAlg;
        switch (mDkimInfo.hashingAlgorithm()) {
        case DKIMInfo::HashingAlgorithmType::Sha1:
            sigAlg = QCA::EMSA3_SHA1;
            break;
        case DKIMInfo::HashingAlgorithmType::Sha256:
            sigAlg = QCA::EMSA3_SHA256;
            break;
        case DKIMInfo::HashingAlgorithmType::Any:
        case DKIMInfo::HashingAlgorithmType::Unknown:
            //Error !
            break;
        }
        if (!rsaPublicKey.verifyMessage(sec, ba, sigAlg, QCA::DERSequence)) {
            computeHeaderCanonization(false);
            const QCA::SecureArray secWithoutQuote = mHeaderCanonizationResult.toLatin1();
            if (!rsaPublicKey.verifyMessage(secWithoutQuote, ba, sigAlg, QCA::DERSequence)) {
                qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Signature invalid";
                // then signature is invalid
                mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::ImpossibleToVerifySignature;
                mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
                Q_EMIT result(createCheckResult());
                deleteLater();
                return;
            }
        }
    } else {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to verify signature";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::ImpossibleToVerifySignature;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid;
    Q_EMIT result(createCheckResult());
    deleteLater();
}

DKIMCheckPolicy DKIMCheckSignatureJob::policy() const
{
    return mPolicy;
}

void DKIMCheckSignatureJob::setPolicy(const DKIMCheckPolicy &policy)
{
    mPolicy = policy;
}

DKIMCheckSignatureJob::DKIMWarning DKIMCheckSignatureJob::warning() const
{
    return mWarning;
}

void DKIMCheckSignatureJob::setWarning(const DKIMCheckSignatureJob::DKIMWarning &warning)
{
    mWarning = warning;
}

KMime::Message::Ptr DKIMCheckSignatureJob::message() const
{
    return mMessage;
}

void DKIMCheckSignatureJob::setMessage(const KMime::Message::Ptr &message)
{
    mMessage = message;
}

MessageViewer::DKIMCheckSignatureJob::DKIMStatus DKIMCheckSignatureJob::checkSignature(const DKIMInfo &info)
{
    const qint64 currentDate = QDateTime::currentSecsSinceEpoch();
    if (info.expireTime() != -1 && info.expireTime() < currentDate) {
        mWarning = DKIMCheckSignatureJob::DKIMWarning::SignatureExpired;
    }
    if (info.signatureTimeStamp() != -1 && info.signatureTimeStamp() > currentDate) {
        mWarning = DKIMCheckSignatureJob::DKIMWarning::SignatureCreatedInFuture;
    }
    if (info.signature().isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Signature doesn't exist";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::MissingSignature;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (!info.listSignedHeader().contains(QLatin1String("from"), Qt::CaseInsensitive)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "From is not include in headers list";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::MissingFrom;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.domain().isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Domain is not defined.";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::DomainNotExist;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.query() != QLatin1String("dns/txt")) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Query is incorrect: " << info.query();
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidQueryMethod;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }

    if ((info.hashingAlgorithm() == MessageViewer::DKIMInfo::HashingAlgorithmType::Any) || (info.hashingAlgorithm() == MessageViewer::DKIMInfo::HashingAlgorithmType::Unknown)) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "body header algorithm is empty";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyHashAlgorithm;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.signingAlgorithm().isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "signature algorithm is empty";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidSignAlgorithm;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }

    if (info.hashingAlgorithm() == DKIMInfo::HashingAlgorithmType::Sha1) {
        if (mPolicy.rsaSha1Policy() == MessageViewer::MessageViewerSettings::EnumPolicyRsaSha1::Nothing) {
            //nothing
        } else if (mPolicy.rsaSha1Policy() == MessageViewer::MessageViewerSettings::EnumPolicyRsaSha1::Warning) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "hash algorithm is not secure sha1 : Error";
            mWarning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe;
        } else if (mPolicy.rsaSha1Policy() == MessageViewer::MessageViewerSettings::EnumPolicyRsaSha1::Error) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "hash algorithm is not secure sha1: Error";
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::HashAlgorithmUnsafeSha1;
            return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        }
    }

    //qDebug() << "info.agentOrUserIdentifier() " << info.agentOrUserIdentifier() << " info.iDomain() " << info.iDomain();
    if (!info.agentOrUserIdentifier().endsWith(info.iDomain())) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "AUID is not in a subdomain of SDID";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::IDomainError;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    //Add more test
    //TODO check if info is valid
    return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid;
}

DKIMCheckSignatureJob::DKIMError DKIMCheckSignatureJob::error() const
{
    return mError;
}

DKIMCheckSignatureJob::DKIMStatus DKIMCheckSignatureJob::status() const
{
    return mStatus;
}

void DKIMCheckSignatureJob::setStatus(const DKIMCheckSignatureJob::DKIMStatus &status)
{
    mStatus = status;
}

QString DKIMCheckSignatureJob::dkimValue() const
{
    return mDkimValue;
}

bool DKIMCheckSignatureJob::CheckSignatureResult::isValid() const
{
    return status != DKIMCheckSignatureJob::DKIMStatus::Unknown;
}

bool DKIMCheckSignatureJob::CheckSignatureResult::operator==(const DKIMCheckSignatureJob::CheckSignatureResult &other) const
{
    return error == other.error
           && warning == other.warning
           && status == other.status
           && fromEmail == other.fromEmail
           && auid == other.auid
           && sdid == other.sdid
           && listSignatureAuthenticationResult == other.listSignatureAuthenticationResult;
}

bool DKIMCheckSignatureJob::CheckSignatureResult::operator!=(const DKIMCheckSignatureJob::CheckSignatureResult &other) const
{
    return !CheckSignatureResult::operator==(other);
}

QDebug operator <<(QDebug d, const DKIMCheckSignatureJob::CheckSignatureResult &t)
{
    d << " error " << t.error;
    d << " warning " << t.warning;
    d << " status " << t.status;
    d << " signedBy " << t.sdid;
    d << " fromEmail " << t.fromEmail;
    d << " auid " << t.auid;
    d << " authenticationResult " << t.listSignatureAuthenticationResult;
    return d;
}

QDebug operator <<(QDebug d, const DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult &t)
{
    d << " method " << t.method;
    d << " errorStr " << t.errorStr;
    d << " status " << t.status;
    d << " sdid " << t.sdid;
    d << " auid " << t.auid;
    return d;
}

bool DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult::operator==(const DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult &other) const
{
    return errorStr == other.errorStr &&
            method == other.method &&
            status == other.status &&
            sdid == other.sdid &&
            auid == other.auid;
}

bool DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult::isValid() const
{
    //TODO improve it
    return (method != AuthenticationMethod::Unknown);
}
