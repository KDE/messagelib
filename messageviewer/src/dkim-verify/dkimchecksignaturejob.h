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
#include "messageviewer_export.h"
#include <MessageViewer/DKIMKeyRecord>
#include <MessageViewer/DKIMInfo>
#include <MessageViewer/DKIMCheckPolicy>
#include <KMime/Message>
#include <AkonadiCore/Item>

namespace MessageViewer {
class MESSAGEVIEWER_EXPORT DKIMCheckSignatureJob : public QObject
{
    Q_OBJECT
public:

    enum class DKIMStatus : int {
        Unknown = 0,
        Valid = 1,
        Invalid = 2,
        EmailNotSigned = 3,
        NeedToBeSigned = 4
    };
    Q_ENUM(DKIMStatus)

    enum class DKIMError : int {
        Any = 0,
        CorruptedBodyHash = 1,
        DomainNotExist = 2,
        MissingFrom = 3,
        MissingSignature = 4,
        InvalidQueryMethod = 5,
        InvalidHeaderCanonicalization = 6,
        InvalidBodyCanonicalization = 7,
        InvalidBodyHashAlgorithm = 8,
        InvalidSignAlgorithm = 9,
        PublicKeyWasRevoked = 10,
        SignatureTooLarge = 11,
        InsupportedHashAlgorithm = 12,
        PublicKeyTooSmall = 13,
        ImpossibleToVerifySignature = 14,
        DomainI = 15,
        TestKeyMode = 16,
        ImpossibleToDownloadKey = 17,
        HashAlgorithmUnsafeSha1 = 18,
        IDomainError = 19,
        PublicKeyConversionError = 20,
    };
    Q_ENUM(DKIMError)
    enum class DKIMWarning : int {
        Any = 0,
        SignatureExpired = 1,
        SignatureCreatedInFuture = 2,
        SignatureTooSmall = 3,
        HashAlgorithmUnsafe = 4,
    };
    Q_ENUM(DKIMWarning)

    struct CheckSignatureResult {
        bool isValid() const
        {
            return status != DKIMCheckSignatureJob::DKIMStatus::Unknown;
        }

        Q_REQUIRED_RESULT bool operator==(const CheckSignatureResult &other) const
        {
            return error == other.error
                   && warning == other.warning
                   && status == other.status
                   && item == other.item
                   && fromEmail == other.fromEmail;
        }

        Q_REQUIRED_RESULT bool operator!=(const CheckSignatureResult &other) const
        {
            return !CheckSignatureResult::operator==(other);
        }

        DKIMCheckSignatureJob::DKIMError error = DKIMCheckSignatureJob::DKIMError::Any;
        DKIMCheckSignatureJob::DKIMWarning warning = DKIMCheckSignatureJob::DKIMWarning::Any;
        DKIMCheckSignatureJob::DKIMStatus status = DKIMCheckSignatureJob::DKIMStatus::Unknown;
        Akonadi::Item item;
        QString signedBy;
        QString fromEmail;
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

    Q_REQUIRED_RESULT QString headerCanonizationResult() const;

    Q_REQUIRED_RESULT QString bodyCanonizationResult() const;

    Q_REQUIRED_RESULT Akonadi::Item item() const;
    void setItem(const Akonadi::Item &item);

    Q_REQUIRED_RESULT bool saveKey() const;
    void setSaveKey(bool saveKey);

    Q_REQUIRED_RESULT DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void storeKey(const QString &key, const QString &domain, const QString &selector);

private:
    void downloadKey(const DKIMInfo &info);
    void slotDownloadKeyDone(const QList<QByteArray> &lst, const QString &domain, const QString &selector);
    void parseDKIMKeyRecord(const QString &str, const QString &domain, const QString &selector, bool storeKeyValue = true);
    Q_REQUIRED_RESULT QString headerCanonizationSimple() const;
    Q_REQUIRED_RESULT QString headerCanonizationRelaxed(bool removeQuoteOnContentType) const;
    Q_REQUIRED_RESULT QString bodyCanonizationRelaxed() const;
    Q_REQUIRED_RESULT QString bodyCanonizationSimple() const;
    Q_REQUIRED_RESULT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult createCheckResult();
    void verifyRSASignature();
    void computeHeaderCanonization(bool removeQuoteOnContentType);
    DKIMCheckPolicy mPolicy;
    KMime::Message::Ptr mMessage;
    Akonadi::Item mMessageItem;
    QString mFromEmail;
    DKIMInfo mDkimInfo;
    DKIMKeyRecord mDkimKeyRecord;
    QString mDkimValue;
    QString mHeaderCanonizationResult;
    QString mBodyCanonizationResult;
    DKIMCheckSignatureJob::DKIMError mError = DKIMCheckSignatureJob::DKIMError::Any;
    DKIMCheckSignatureJob::DKIMWarning mWarning = DKIMCheckSignatureJob::DKIMWarning::Any;
    DKIMCheckSignatureJob::DKIMStatus mStatus = DKIMCheckSignatureJob::DKIMStatus::Unknown;
    bool mSaveKey = false;
};
}
Q_DECLARE_METATYPE(MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult)
#endif // DKIMCHECKSIGNATUREJOB_H
