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
#include "dkiminfo.h"
#include "dkimkeyrecord.h"
#include "messageviewer_debug.h"
#include <KLocalizedString>
#include <QDateTime>
//see https://tools.ietf.org/html/rfc6376

using namespace MessageViewer;
DKIMCheckSignatureJob::DKIMCheckSignatureJob(QObject *parent)
    : QObject(parent)
{
}

DKIMCheckSignatureJob::~DKIMCheckSignatureJob()
{
}

void DKIMCheckSignatureJob::start()
{
    if (!mMessage) {
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        deleteLater();
        return;
    }
    if (auto hrd = mMessage->headerByType("DKIM-Signature")) {
        mDkimValue = hrd->asUnicodeString();
    }
    if (mDkimValue.isEmpty()) {
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned);
        deleteLater();
        return;
    }
    if (!mDkimInfo.parseDKIM(mDkimValue)) {
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to parse header" << mDkimValue;
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        deleteLater();
        return;
    }
    const MessageViewer::DKIMCheckSignatureJob::DKIMStatus status = checkSignature(mDkimInfo);
    if (status != MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Valid) {
        Q_EMIT result(status);
        deleteLater();
        return;
    }
    //ComputeBodyHash now.
    QString bodyCanonizationResult;
    switch (mDkimInfo.bodyCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyCanonicalization;
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        deleteLater();
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        bodyCanonizationResult = bodyCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        bodyCanonizationResult = bodyCanonizationRelaxed();
        break;
    }

    if (mDkimInfo.bodyLenghtCount() != -1) { //Verify it.
        if (mDkimInfo.bodyLenghtCount() < bodyCanonizationResult.length()) {
            //TODO
        } else if (mDkimInfo.bodyLenghtCount() > bodyCanonizationResult.length()) {
            //TODO
        }
        // truncated body to the length specified in the "l=" tag
        // TODO
    }

    //Compute Hash Header
    QString headerCanonizationResult;
    switch (mDkimInfo.headerCanonization()) {
    case MessageViewer::DKIMInfo::CanonicalizationType::Unknown:
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidHeaderCanonicalization;
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        deleteLater();
        return;
    case MessageViewer::DKIMInfo::CanonicalizationType::Simple:
        headerCanonizationResult = headerCanonizationSimple();
        break;
    case MessageViewer::DKIMInfo::CanonicalizationType::Relaxed:
        headerCanonizationResult = headerCanonizationRelaxed();
        break;
    }

    downloadKey(mDkimInfo);
}

QString DKIMCheckSignatureJob::bodyCanonizationSimple() const
{
    return {};
}

QString DKIMCheckSignatureJob::bodyCanonizationRelaxed() const
{
    return {};
}

QString DKIMCheckSignatureJob::headerCanonizationSimple() const
{
    for (const QString &header : mDkimInfo.listSignedHeader()) {
        if (auto hrd = mMessage->headerByType(header.toLatin1().constData())) {
            //TODO
        }
    }
    return {};
}

QString DKIMCheckSignatureJob::headerCanonizationRelaxed() const
{
    return {};
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
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to start downloadkey";
        deleteLater();
    }
}

void DKIMCheckSignatureJob::slotDownloadKeyDone(const QList<QByteArray> &lst)
{
    if (lst.count() != 1) {
        qCWarning(MESSAGEVIEWER_LOG) << "Key result has more that 1 element";
        deleteLater();
        return;
    }
    parseDKIMKeyRecord(QString::fromLocal8Bit(lst.at(0)));
}

void DKIMCheckSignatureJob::parseDKIMKeyRecord(const QString &str)
{
    if (!mDkimKeyRecord.parseKey(str)) {
        qCWarning(MESSAGEVIEWER_LOG) << "Impossible to parse key record " << str;
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
        deleteLater();
        return;
    }

    if (mDkimKeyRecord.keyType() != QLatin1String("rsa")) {
        qCWarning(MESSAGEVIEWER_LOG) << "mDkimKeyRecord key type is unknown " << mDkimKeyRecord.keyType();
        Q_EMIT result(MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid);
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

    verifyRSASignature();
}

void DKIMCheckSignatureJob::verifyRSASignature()
{
    //TODO
    deleteLater();
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
        mWarningFound = i18n("Signature Expired");
    }
    if (info.signatureTimeStamp() != -1 && info.signatureTimeStamp() > currentDate) {
        mWarningFound += i18n("Signature created in the future");
    }
    if (info.signature().isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Signature doesn't exist";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::MissingSignature;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (!info.listSignedHeader().contains(QLatin1String("from"))) {
        qCWarning(MESSAGEVIEWER_LOG) << "From is not include in headers list";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::MissingFrom;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.domain().isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Domain is not defined.";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::DomainNotExist;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.query() != QLatin1String("dns/txt")) {
        qCWarning(MESSAGEVIEWER_LOG) << "Query is incorrect: " << info.query();
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidQueryMethod;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }

    if (info.hashingAlgorithm().isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "body header algorithm is empty";
        mError = MessageViewer::DKIMCheckSignatureJob::DKIMError::InvalidBodyHashAlgorithm;
        return MessageViewer::DKIMCheckSignatureJob::DKIMStatus::Invalid;
    }
    if (info.signingAlgorithm().isEmpty()) {
        qCWarning(MESSAGEVIEWER_LOG) << "signature algorithm is empty";
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

QString DKIMCheckSignatureJob::warningFound() const
{
    return mWarningFound;
}

void DKIMCheckSignatureJob::setWarningFound(const QString &warningFound)
{
    mWarningFound = warningFound;
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
