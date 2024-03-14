/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimwidgetinfo.h"
#include "dkimmanager.h"
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KColorScheme>
#include <KLocalizedString>

#include <QHBoxLayout>
#include <QLabel>

using namespace MessageViewer;
DKIMWidgetInfo::DKIMWidgetInfo(QWidget *parent)
    : QWidget(parent)
    , mLabel(new QLabel(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mLabel->setAutoFillBackground(true);
    mLabel->setObjectName(QLatin1StringView("label"));
    mainLayout->addWidget(mLabel);
    connect(DKIMManager::self(), &DKIMManager::result, this, &DKIMWidgetInfo::setResult);
    connect(DKIMManager::self(), &DKIMManager::clearInfo, this, &DKIMWidgetInfo::clear);
    initColors();
}

DKIMWidgetInfo::~DKIMWidgetInfo() = default;

void DKIMWidgetInfo::updatePalette()
{
    initColors();
    updateInfo();
}

void DKIMWidgetInfo::initColors()
{
    const KColorScheme colorScheme{QPalette::Active};
    mWarningColor = colorScheme.background(KColorScheme::NeutralBackground).color();
    mErrorColor = colorScheme.background(KColorScheme::NegativeBackground).color();
    mOkColor = colorScheme.background(KColorScheme::PositiveBackground).color();
    mDefaultColor = Qt::transparent; // colorScheme.background(KColorScheme::ActiveBackground).color();
}

MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult DKIMWidgetInfo::result() const
{
    return mResult;
}

Akonadi::Item::Id DKIMWidgetInfo::currentItemId() const
{
    return mCurrentItemId;
}

void DKIMWidgetInfo::setCurrentItemId(Akonadi::Item::Id currentItemId)
{
    mCurrentItemId = currentItemId;
}

void DKIMWidgetInfo::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id)
{
    if (mCurrentItemId == id) {
        if (mResult != checkResult) {
            mResult = checkResult;
            updateInfo();
        }
    }
}

void DKIMWidgetInfo::clear()
{
    mLabel->clear();
    mLabel->setToolTip(QString());
    QPalette pal = mLabel->palette();
    pal.setColor(backgroundRole(), mDefaultColor);
    mLabel->setPalette(pal);
    mCurrentItemId = -1;
    mResult = {};
}

void DKIMWidgetInfo::updateInfo()
{
    QPalette pal = mLabel->palette();
    switch (mResult.status) {
    case DKIMCheckSignatureJob::DKIMStatus::Unknown:
        pal.setColor(backgroundRole(), mDefaultColor);
        mLabel->setPalette(pal);
        mLabel->setText(i18n("Unknown"));
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Valid:
        if (mResult.sdid.isEmpty()) {
            qCWarning(MESSAGEVIEWER_DKIMCHECKER_LOG) << "mResult.sdid is empty. It's a bug";
        }
        mLabel->setText(i18n("DKIM: valid (signed by %1)", mResult.sdid));
        pal.setColor(backgroundRole(), (mResult.warning != DKIMCheckSignatureJob::DKIMWarning::Any) ? mWarningColor : mOkColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Invalid:
        pal.setColor(backgroundRole(), mErrorColor);
        mLabel->setPalette(pal);
        mLabel->setText(i18n("DKIM: invalid"));
        break;
    case DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned:
        mLabel->setText(i18n("DKIM: Not signed"));
        pal.setColor(backgroundRole(), mDefaultColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned:
        mLabel->setText(i18n("DKIM: Should Be Signed by %1", mResult.sdid));
        pal.setColor(backgroundRole(), mErrorColor);
        mLabel->setPalette(pal);
        break;
    }
    updateToolTip();
}

void DKIMWidgetInfo::updateToolTip()
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
    qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "mResult.authentication " << mResult.listSignatureAuthenticationResult;

    mLabel->setToolTip(tooltip);
}

#include "moc_dkimwidgetinfo.cpp"
