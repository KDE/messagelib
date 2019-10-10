/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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
#include "settings/messageviewersettings.h"
#include <QDateTime>
#include <QCryptographicHash>
//see https://tools.ietf.org/html/rfc6376

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
    return result;
}


void DKIMCheckSignatureJob::start()
{
    if (!mMessage) {
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        createCheckResult();
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
    if (auto hrd = mMessage->headerByType("DKIM-Signature")) {
        mDkimValue = hrd->asUnicodeString();
    }
    if (mDkimValue.isEmpty()) {
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }
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
    QString bodyCanonizationResult;
    switch (mDkimInfo.bodyCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyCanonicalization;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        bodyCanonizationResult = bodyCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        bodyCanonizationResult = bodyCanonizationRelaxed();
        break;
    }
    QByteArray resultHash;
    if (mDkimInfo.hashingAlgorithm() == QLatin1String("sha1")) {
        resultHash = MessageViewer::DKIMUtil::generateHash(bodyCanonizationResult.toLatin1(), QCryptographicHash::Sha1);
    } else if (mDkimInfo.hashingAlgorithm() == QLatin1String("sha256")) {
        resultHash = MessageViewer::DKIMUtil::generateHash(bodyCanonizationResult.toLatin1(), QCryptographicHash::Sha256);
    } else {
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InsupportedHashAlgorithm;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    }

    qDebug() << " bodyCanonizationResult "<< bodyCanonizationResult << resultHash.toBase64() << " algorithm " << mDkimInfo.hashingAlgorithm() << mDkimInfo.bodyHash();

    if (mDkimInfo.bodyLenghtCount() != -1) { //Verify it.
        if (mDkimInfo.bodyLenghtCount() < bodyCanonizationResult.length()) {
            // lenght tag exceeds body size
            mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::SignatureTooLarge;
            mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
            Q_EMIT result(createCheckResult());
            deleteLater();
            return;
        } else if (mDkimInfo.bodyLenghtCount() > bodyCanonizationResult.length()) {
            mWarning = MessageViewer::DKIMCheckSignatureJob::DKIMWarning::SignatureTooSmall;
        }
        // truncated body to the length specified in the "l=" tag
        bodyCanonizationResult = bodyCanonizationResult.left(mDkimInfo.bodyLenghtCount());
    }

    //Compute Hash Header
    QString headerCanonizationResult;
    switch (mDkimInfo.headerCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidHeaderCanonicalization;
        mStatus = MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
        Q_EMIT result(createCheckResult());
        deleteLater();
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        headerCanonizationResult = headerCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        headerCanonizationResult = headerCanonizationRelaxed();
        break;
    }

    qDebug() << " headerCanonizationResult" << headerCanonizationResult;
    if (MessageViewer::MessageViewerSettings::self()->saveKey()) {
        const QString keyValue = MessageViewer::DKIMManagerKey::self()->keyValue(mDkimInfo.selector(), mDkimInfo.domain());
        if (keyValue.isEmpty()) {
            downloadKey(mDkimInfo);
        } else {
            parseDKIMKeyRecord(keyValue, mDkimInfo.domain(), mDkimInfo.selector(), false);
        }
    }
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

    return MessageViewer::DKIMUtil::bodyCanonizationSimple(QString::fromLatin1(mMessage->body()));
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
    return MessageViewer::DKIMUtil::bodyCanonizationRelaxed(QString::fromUtf8(mMessage->body()));
}

QString DKIMCheckSignatureJob::headerCanonizationSimple() const
{
    QString headers;
    for (const QString &header : mDkimInfo.listSignedHeader()) {
        if (auto hrd = mMessage->headerByType(header.toLatin1().constData())) {
            headers += MessageViewer::DKIMUtil::headerCanonizationSimple(header, hrd->asUnicodeString());
        }
    }
    return headers;
}

QString DKIMCheckSignatureJob::headerCanonizationRelaxed() const
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
    for (const QString &header : mDkimInfo.listSignedHeader()) {
        if (auto hrd = mMessage->headerByType(header.toLatin1().constData())) {
            if (!headers.isEmpty()) {
                headers += QLatin1String("\r\n");
            }
            headers += MessageViewer::DKIMUtil::headerCanonizationRelaxed(header, hrd->asUnicodeString());
        }
    }
    return headers;
}

void DKIMCheckSignatureJob::downloadKey(const DKIMInfo &info)
{
    DKIMDownloadKeyJob *job = new DKIMDownloadKeyJob(this);
    job->setDomainName(info.domain());
    job->setSelectorName(info.selector());
    connect(job, &DKIMDownloadKeyJob::error, this, [this](){
        deleteLater();
    });
    connect(job, &DKIMDownloadKeyJob::success, this, &DKIMCheckSignatureJob::slotDownloadKeyDone);

    if (!job->start()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "Impossible to start downloadkey";
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

    if (mDkimKeyRecord.flags().contains(QLatin1String("s"))) {
        //                  s  Any DKIM-Signature header fields using the "i=" tag MUST have
        //                     the same domain value on the right-hand side of the "@" in the
        //                     "i=" tag and the value of the "d=" tag.  That is, the "i="
        //                     domain MUST NOT be a subdomain of "d=".  Use of this flag is
        //                     RECOMMENDED unless subdomaining is required.
        //TODO
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
    qDebug() << " void DKIMCheckSignatureJob::verifyRSASignature() not implemented yet";
    //TODO
    deleteLater();
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

    if (info.hashingAlgorithm().isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "body header algorithm is empty";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyHashAlgorithm;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.signingAlgorithm().isEmpty()) {
        qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "signature algorithm is empty";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidSignAlgorithm;
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
