/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmailstatus.h"
#include "dkimmanager.h"
#include "dkimutil.h"
#include <KLocalizedString>

using namespace MessageViewer;
DKIMMailStatus::DKIMMailStatus(QObject *parent)
    : QObject(parent)
{
    connect(DKIMManager::self(), &DKIMManager::result, this, &DKIMMailStatus::setResult);
    connect(DKIMManager::self(), &DKIMManager::clearInfo, this, &DKIMMailStatus::clear);
}

MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult DKIMMailStatus::result() const
{
    return mResult;
}

Akonadi::Item::Id DKIMMailStatus::currentItemId() const
{
    return mCurrentItemId;
}

void DKIMMailStatus::setCurrentItemId(Akonadi::Item::Id currentItemId)
{
    if (currentItemId == mCurrentItemId) {
        return;
    }
    mCurrentItemId = currentItemId;
    Q_EMIT currentItemIdChanged();
}

void DKIMMailStatus::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id)
{
    if (mCurrentItemId == id) {
        if (mResult != checkResult) {
            mResult = checkResult;
            Q_EMIT textChanged();
            Q_EMIT tooltipChanged();
            Q_EMIT statusChanged();
            Q_EMIT errorChanged();
        }
    }
}

void DKIMMailStatus::clear()
{
    mCurrentItemId = -1;
    mResult = {};
    Q_EMIT textChanged();
    Q_EMIT tooltipChanged();
    Q_EMIT statusChanged();
    Q_EMIT errorChanged();
    Q_EMIT currentItemIdChanged();
}

QString DKIMMailStatus::text() const
{
    switch (mResult.status) {
    case DKIMCheckSignatureJob::DKIMStatus::Unknown:
        return i18n("Unknown");
    case DKIMCheckSignatureJob::DKIMStatus::Valid:
        if (mResult.sdid.isEmpty()) {
            qWarning() << "mResult.sdid is empty. It's a bug";
        }
        return i18n("DKIM: valid (signed by %1)", mResult.sdid);
    case DKIMCheckSignatureJob::DKIMStatus::Invalid:
        return i18n("DKIM: invalid");
    case DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned:
        return i18n("DKIM: Not signed");
    case DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned:
        return i18n("DKIM: Should Be Signed by %1", mResult.sdid);
    }
    return QString();
}

QString DKIMMailStatus::tooltip() const
{
    QString tooltip;
    if (mResult.status == DKIMCheckSignatureJob::DKIMStatus::Invalid) {
        switch (mResult.error) {
        case DKIMCheckSignatureJob::DKIMError::Any:
            break;
        case DKIMCheckSignatureJob::DKIMError::CorruptedBodyHash:
            tooltip = i18n("Body Hash was corrupted.");
            break;
        case DKIMCheckSignatureJob::DKIMError::DomainNotExist:
            tooltip = i18n("The domain doesn't exist.");
            break;
        case DKIMCheckSignatureJob::DKIMError::MissingFrom:
            tooltip = i18n("Missing header From.");
            break;
        case DKIMCheckSignatureJob::DKIMError::MissingSignature:
            tooltip = i18n("Missing signature.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidQueryMethod:
            tooltip = i18n("Invalid query method.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidHeaderCanonicalization:
            tooltip = i18n("Invalid header canonicalization.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidBodyCanonicalization:
            tooltip = i18n("Invalid body canonicalization.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidBodyHashAlgorithm:
            tooltip = i18n("Unknown Body Hash Algorithm.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidSignAlgorithm:
            tooltip = i18n("Signature algorithm is invalid.");
            break;
        case DKIMCheckSignatureJob::DKIMError::PublicKeyWasRevoked:
            tooltip = i18n("The public key was revoked.");
            break;
        case DKIMCheckSignatureJob::DKIMError::SignatureTooLarge:
            tooltip = i18n("Signature is too large.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InsupportedHashAlgorithm:
            tooltip = i18n("Hash Algorithm is unsupported.");
            break;
        case DKIMCheckSignatureJob::DKIMError::PublicKeyTooSmall:
            tooltip = i18n("Public key is too small.");
            break;
        case DKIMCheckSignatureJob::DKIMError::ImpossibleToVerifySignature:
            tooltip = i18n("Impossible to verify signature.");
            break;
        case DKIMCheckSignatureJob::DKIMError::DomainI:
            tooltip = i18n("AUID must be in the same domain as SDID (s-flag set in key record).");
            break;
        case DKIMCheckSignatureJob::DKIMError::TestKeyMode:
            tooltip = i18n("The signing domain is only testing DKIM.");
            break;
        case DKIMCheckSignatureJob::DKIMError::ImpossibleToDownloadKey:
            tooltip = i18n("Impossible to download key.");
            break;
        case DKIMCheckSignatureJob::DKIMError::HashAlgorithmUnsafeSha1:
            tooltip = i18n("Hash Algorithm unsafe (sha1)");
            break;
        case DKIMCheckSignatureJob::DKIMError::IDomainError:
            tooltip = i18n("AUID is not in a subdomain of SDID");
            break;
        case DKIMCheckSignatureJob::DKIMError::PublicKeyConversionError:
            tooltip = i18n("Problem during converting public key");
            break;
        }
    }

    switch (mResult.warning) {
    case DKIMCheckSignatureJob::DKIMWarning::Any:
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureExpired:
        tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Signature expired");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureCreatedInFuture:
        tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Signature created in the future");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureTooSmall:
        tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Signature too small");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe:
        tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Hash Algorithm unsafe (sha1)");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::PublicRsaKeyTooSmall:
        tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Public Key too small");
        break;
    }

    if (mResult.status != DKIMCheckSignatureJob::DKIMStatus::Invalid) {
        QStringList tooltipList;
        for (const DKIMCheckSignatureJob::DKIMCheckSignatureAuthenticationResult &result : std::as_const(mResult.listSignatureAuthenticationResult)) {
            switch (result.status) {
            case DKIMCheckSignatureJob::DKIMStatus::Unknown:
                break;
            case DKIMCheckSignatureJob::DKIMStatus::Invalid:
                switch (result.method) {
                case DKIMCheckSignatureJob::AuthenticationMethod::Unknown: {
                    break;
                }
                case DKIMCheckSignatureJob::AuthenticationMethod::Spf:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkim:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dmarc:
                case DKIMCheckSignatureJob::AuthenticationMethod::Auth:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps: {
                    const QString str = i18nc("method name: info about it from parsing",
                                              "%1: %2",
                                              MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(result.method),
                                              result.infoResult);
                    if (!tooltipList.contains(str)) {
                        tooltipList.append(str);
                    }
                    break;
                }
                }
                break;

            case DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned:
                break;
            case DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned:
                switch (result.method) {
                case DKIMCheckSignatureJob::AuthenticationMethod::Unknown: {
                    break;
                }
                case DKIMCheckSignatureJob::AuthenticationMethod::Spf:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkim:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dmarc:
                case DKIMCheckSignatureJob::AuthenticationMethod::Auth:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps: {
                    const QString str = i18n("%1: None", MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(result.method));
                    if (!tooltipList.contains(str)) {
                        tooltipList.append(str);
                    }
                    break;
                }
                }
                break;
            case DKIMCheckSignatureJob::DKIMStatus::Valid:
                switch (result.method) {
                case DKIMCheckSignatureJob::AuthenticationMethod::Unknown: {
                    break;
                }
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkim: {
                    const QString str =
                        i18n("%1: Valid (Signed by %2)", MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(result.method), result.sdid);
                    if (!tooltipList.contains(str)) {
                        tooltipList.append(str);
                    }
                    break;
                }
                case DKIMCheckSignatureJob::AuthenticationMethod::Spf: {
                    const QString str = i18nc("method name: info about it from parsing",
                                              "%1: %2",
                                              MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(result.method),
                                              result.infoResult);
                    if (!tooltipList.contains(str)) {
                        tooltipList.append(str);
                    }

                    break;
                }
                case DKIMCheckSignatureJob::AuthenticationMethod::Auth:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dkimatps:
                case DKIMCheckSignatureJob::AuthenticationMethod::Dmarc: {
                    const QString str = i18n("%1: Valid", MessageViewer::DKIMUtil::convertAuthenticationMethodEnumToString(result.method));
                    if (!tooltipList.contains(str)) {
                        tooltipList.append(str);
                    }

                    break;
                }
                }
                break;
            }
        }
        if (!tooltipList.isEmpty()) {
            tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + tooltipList.join(QLatin1Char('\n'));
        }
        if (mResult.listSignatureAuthenticationResult.isEmpty()) {
            tooltip += (tooltip.isEmpty() ? QChar() : QLatin1Char('\n')) + i18n("Not Signed");
        }
    }
    qDebug() << "mResult.authentication " << mResult.listSignatureAuthenticationResult;

    return tooltip;
}

DKIMCheckSignatureJob::DKIMStatus DKIMMailStatus::status() const
{
    return mResult.status;
}

DKIMCheckSignatureJob::DKIMError DKIMMailStatus::error() const
{
    return mResult.error;
}
