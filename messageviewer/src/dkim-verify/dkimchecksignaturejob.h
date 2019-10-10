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

#ifndef DKIMCHECKSIGNATUREJOB_H
#define DKIMCHECKSIGNATUREJOB_H

#include <QObject>
#include "dkimkeyrecord.h"
#include "dkiminfo.h"
#include "messageviewer_export.h"
#include <KMime/Message>

namespace MessageViewer {
class MESSAGEVIEWER_EXPORT DKIMCheckSignatureJob : public QObject
{
    Q_OBJECT
public:


    enum class DKIMStatus {
        Unknown,
        Valid,
        Invalid,
        EmailNotSigned
    };

    enum class DKIMError {
        Any,
        CorruptedBodyHash,
        DomainNotExist,
        MissingFrom,
        MissingSignature,
        InvalidQueryMethod,
        InvalidHeaderCanonicalization,
        InvalidBodyCanonicalization,
        InvalidBodyHashAlgorithm,
        InvalidSignAlgorithm,
        PublicKeyWasRevoked,
        SignatureTooLarge,
        InsupportedHashAlgorithm,

        //TODO add more
    };
    enum class DKIMWarning {
        Any,
        SignatureExpired,
        SignatureCreatedInFuture,
        SignatureTooSmall
    };

    struct CheckSignatureResult {
        DKIMCheckSignatureJob::DKIMError error = DKIMCheckSignatureJob::DKIMError::Any;
        DKIMCheckSignatureJob::DKIMWarning warning = DKIMCheckSignatureJob::DKIMWarning::Any;
        DKIMCheckSignatureJob::DKIMStatus status = DKIMCheckSignatureJob::DKIMStatus::Unknown;
    };

    explicit DKIMCheckSignatureJob(QObject *parent = nullptr);
    ~DKIMCheckSignatureJob();
    void start();

    Q_REQUIRED_RESULT QString dkimValue() const;

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::DKIMStatus status() const;
    void setStatus(const DKIMCheckSignatureJob::DKIMStatus &status);

    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::DKIMStatus checkSignature(const MessageViewer::DKIMInfo &info);

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::DKIMError error() const;

    Q_REQUIRED_RESULT KMime::Message::Ptr message() const;
    void setMessage(const KMime::Message::Ptr &message);

    Q_REQUIRED_RESULT DKIMCheckSignatureJob::DKIMWarning warning() const;
    void setWarning(const DKIMWarning &warning);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void storeKey(const QString &key, const QString &domain, const QString &selector);

private:
    void downloadKey(const DKIMInfo &info);
    void slotDownloadKeyDone(const QList<QByteArray> &lst, const QString &domain, const QString &selector);
    void parseDKIMKeyRecord(const QString &str, const QString &domain, const QString &selector, bool storeKeyValue = true);
    QString headerCanonizationSimple() const;
    QString headerCanonizationRelaxed() const;
    QString bodyCanonizationRelaxed() const;
    QString bodyCanonizationSimple() const;
    void verifyRSASignature();
    MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult createCheckResult();
    KMime::Message::Ptr mMessage;
    DKIMInfo mDkimInfo;
    DKIMKeyRecord mDkimKeyRecord;
    QString mDkimValue;
    DKIMCheckSignatureJob::DKIMError mError = DKIMCheckSignatureJob::DKIMError::Any;
    DKIMCheckSignatureJob::DKIMWarning mWarning = DKIMCheckSignatureJob::DKIMWarning::Any;
    DKIMCheckSignatureJob::DKIMStatus mStatus = DKIMCheckSignatureJob::DKIMStatus::Unknown;
};
}

#endif // DKIMCHECKSIGNATUREJOB_H
