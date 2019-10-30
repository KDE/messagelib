/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "dkimwidgetinfo.h"
#include "dkimmanager.h"
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QLabel>
#include <KColorScheme>
using namespace MessageViewer;
DKIMWidgetInfo::DKIMWidgetInfo(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mLabel = new QLabel(this);
    mLabel->setAutoFillBackground(true);
    mLabel->setObjectName(QStringLiteral("label"));
    mainLayout->addWidget(mLabel);
    connect(DKIMManager::self(), &DKIMManager::result, this, &DKIMWidgetInfo::setResult);
    connect(DKIMManager::self(), &DKIMManager::clearInfo, this, &DKIMWidgetInfo::clear);
    initColors();
}

DKIMWidgetInfo::~DKIMWidgetInfo()
{
}

void DKIMWidgetInfo::initColors()
{
    const KColorScheme colorScheme {QPalette::Active};
    mWarningColor = colorScheme.background(KColorScheme::NeutralBackground).color();
    mErrorColor = colorScheme.background(KColorScheme::NegativeBackground).color();
    mOkColor = colorScheme.background(KColorScheme::PositiveBackground).color();
    mDefaultColor = palette().window().color();
}

Akonadi::Item::Id DKIMWidgetInfo::currentItemId() const
{
    return mCurrentItemId;
}

void DKIMWidgetInfo::setCurrentItemId(const Akonadi::Item::Id &currentItemId)
{
    mCurrentItemId = currentItemId;
}

void DKIMWidgetInfo::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mCurrentItemId == checkResult.item.id()) {
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
        mLabel->setText(i18n("DKIM: valid (signed by %1)", mResult.signedBy));
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
        mLabel->setText(i18n("DKIM: Should Be Signed by %1", mResult.signedBy));
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
        tooltip += QLatin1Char('\n') + i18n("Signature expired");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureCreatedInFuture:
        tooltip += QLatin1Char('\n') + i18n("Signature created in the future");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureTooSmall:
        tooltip += QLatin1Char('\n') + i18n("Signature too small");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::HashAlgorithmUnsafe:
        tooltip += QLatin1Char('\n') + i18n("Hash Algorithm unsafe (sha1)");
        break;
    }
    mLabel->setToolTip(tooltip);
}
