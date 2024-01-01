/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <KMime/Message>
#include <MessageViewer/DKIMCheckPolicy>
#include <MessageViewer/DKIMHeaderParser>
#include <MessageViewer/DKIMInfo>
#include <MessageViewer/DKIMKeyRecord>
#include <QObject>

namespace MessageViewer
{
/**
 * @brief The DKIMCheckSignatureJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMCheckSignatureJob : public QObject
{
    Q_OBJECT
public:
    enum class DKIMStatus : int {
        Unknown = 0,
        Valid = 1,
        Invalid = 2,
        EmailNotSigned = 3,
        NeedToBeSigned = 4,
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
        PublicRsaKeyTooSmall = 5,
    };
    Q_ENUM(DKIMWarning)

    enum class AuthenticationMethod : int {
        Unknown = 0,
        Dkim = 1,
        Spf = 2,
        Dmarc = 3,
        Dkimatps = 4,
        Auth = 5,
    };
    Q_ENUM(AuthenticationMethod)

    struct MESSAGEVIEWER_EXPORT DKIMCheckSignatureAuthenticationResult {
        QString errorStr;
        QString infoResult;
        AuthenticationMethod method = AuthenticationMethod::Unknown;
        DKIMCheckSignatureJob::DKIMStatus status = DKIMCheckSignatureJob::DKIMStatus::Unknown;
        QString sdid; // Signing Domain Identifier
        QString auid; // DKIM MAY optionally provide a single responsible Agent or User Identifier (AUID).
        [[nodiscard]] bool operator==(const DKIMCheckSignatureAuthenticationResult &other) const;
        [[nodiscard]] bool isValid() const;
    };

    struct MESSAGEVIEWER_EXPORT CheckSignatureResult {
        [[nodiscard]] bool isValid() const;

        [[nodiscard]] bool operator==(const CheckSignatureResult &other) const;

        [[nodiscard]] bool operator!=(const CheckSignatureResult &other) const;

        DKIMCheckSignatureJob::DKIMError error = DKIMCheckSignatureJob::DKIMError::Any;
        DKIMCheckSignatureJob::DKIMWarning warning = DKIMCheckSignatureJob::DKIMWarning::Any;
        DKIMCheckSignatureJob::DKIMStatus status = DKIMCheckSignatureJob::DKIMStatus::Unknown;
        QString sdid; // Signing Domain Identifier
        QString auid; // DKIM MAY optionally provide a single responsible Agent or User Identifier (AUID).
        QString fromEmail;

        QList<DKIMCheckSignatureAuthenticationResult> listSignatureAuthenticationResult;
    };

    explicit DKIMCheckSignatureJob(QObject *parent = nullptr);
    ~DKIMCheckSignatureJob() override;
    void start();

    [[nodiscard]] QString dkimValue() const;

    [[nodiscard]] DKIMCheckSignatureJob::DKIMStatus status() const;
    void setStatus(MessageViewer::DKIMCheckSignatureJob::DKIMStatus status);

    [[nodiscard]] MessageViewer::DKIMCheckSignatureJob::DKIMStatus checkSignature(const MessageViewer::DKIMInfo &info);

    [[nodiscard]] DKIMCheckSignatureJob::DKIMError error() const;

    [[nodiscard]] KMime::Message::Ptr message() const;
    void setMessage(const KMime::Message::Ptr &message);

    [[nodiscard]] DKIMCheckSignatureJob::DKIMWarning warning() const;
    void setWarning(MessageViewer::DKIMCheckSignatureJob::DKIMWarning warning);

    [[nodiscard]] QString headerCanonizationResult() const;

    [[nodiscard]] QString bodyCanonizationResult() const;

    [[nodiscard]] DKIMCheckPolicy policy() const;
    void setPolicy(const DKIMCheckPolicy &policy);

    void setHeaderParser(const DKIMHeaderParser &headerParser);

    void setCheckSignatureAuthenticationResult(const QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> &lst);

Q_SIGNALS:
    void result(const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &checkResult);
    void storeKey(const QString &key, const QString &domain, const QString &selector);

private:
    MESSAGEVIEWER_NO_EXPORT void downloadKey(const DKIMInfo &info);
    MESSAGEVIEWER_NO_EXPORT void slotDownloadKeyDone(const QList<QByteArray> &lst, const QString &domain, const QString &selector);
    MESSAGEVIEWER_NO_EXPORT void parseDKIMKeyRecord(const QString &str, const QString &domain, const QString &selector, bool storeKeyValue = true);
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString headerCanonizationSimple() const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString headerCanonizationRelaxed(bool removeQuoteOnContentType) const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString bodyCanonizationRelaxed() const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString bodyCanonizationSimple() const;
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult createCheckResult() const;
    MESSAGEVIEWER_NO_EXPORT void verifySignature();
    MESSAGEVIEWER_NO_EXPORT void verifyRSASignature();
    MESSAGEVIEWER_NO_EXPORT void verifyEd25519Signature();
    MESSAGEVIEWER_NO_EXPORT void computeHeaderCanonization(bool removeQuoteOnContentType);
    QList<DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult> mCheckSignatureAuthenticationResult;
    DKIMCheckPolicy mPolicy;
    DKIMHeaderParser mHeaderParser;
    KMime::Message::Ptr mMessage;
    QString mFromEmail;
    DKIMInfo mDkimInfo;
    DKIMKeyRecord mDkimKeyRecord;
    QString mDkimValue;
    QString mHeaderCanonizationResult;
    QString mBodyCanonizationResult;
    DKIMCheckSignatureJob::DKIMError mError = DKIMCheckSignatureJob::DKIMError::Any;
    DKIMCheckSignatureJob::DKIMWarning mWarning = DKIMCheckSignatureJob::DKIMWarning::Any;
    DKIMCheckSignatureJob::DKIMStatus mStatus = DKIMCheckSignatureJob::DKIMStatus::Unknown;
};
}
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult &t);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult &t);
Q_DECLARE_METATYPE(MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult)
Q_DECLARE_TYPEINFO(MessageViewer::DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult, Q_RELOCATABLE_TYPE);
