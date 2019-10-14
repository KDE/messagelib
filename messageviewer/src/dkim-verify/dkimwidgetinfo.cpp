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
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QLabel>
using namespace MessageViewer;
DKIMWidgetInfo::DKIMWidgetInfo(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mLabel = new QLabel(this);
    mLabel->setObjectName(QStringLiteral("label"));
    mainLayout->addWidget(mLabel);
}

DKIMWidgetInfo::~DKIMWidgetInfo()
{
}

void DKIMWidgetInfo::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult)
{
    if (mResult != checkResult) {
        mResult = checkResult;
        updateInfo();
    }
}

void DKIMWidgetInfo::updateInfo()
{
    switch (mResult.status) {
    case DKIMCheckSignatureJob::DKIMStatus::Unknown:
        mLabel->setText(i18n("Unknown"));
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Valid:
        mLabel->setText(i18n("KDIM: valid"));
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Invalid:
        mLabel->setText(i18n("KDIM: invalid"));
        break;
    case DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned:
        mLabel->setText(i18n("KDIM: Unsigned"));
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
            break;
        case DKIMCheckSignatureJob::DKIMError::MissingFrom:
            tooltip = i18n("Missing header From.");
            break;
        case DKIMCheckSignatureJob::DKIMError::MissingSignature:
            tooltip = i18n("Missing signature.");
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidQueryMethod:
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidHeaderCanonicalization:
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidBodyCanonicalization:
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidBodyHashAlgorithm:
            break;
        case DKIMCheckSignatureJob::DKIMError::InvalidSignAlgorithm:
            break;
        case DKIMCheckSignatureJob::DKIMError::PublicKeyWasRevoked:
            tooltip = i18n("The public key was revoked.");
            break;
        case DKIMCheckSignatureJob::DKIMError::SignatureTooLarge:
            break;
        case DKIMCheckSignatureJob::DKIMError::InsupportedHashAlgorithm:
            tooltip = i18n("Hash Algorithm is unsupported.");
            break;
        case DKIMCheckSignatureJob::DKIMError::PublicKeyTooSmall:
            break;
        case DKIMCheckSignatureJob::DKIMError::ImpossibleToVerifySignature:
            break;
        }
    }

    switch (mResult.warning) {
    case DKIMCheckSignatureJob::DKIMWarning::Any:
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureExpired:
        tooltip = i18n("Signature expired");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureCreatedInFuture:
        tooltip = i18n("Signature created in the future");
        break;
    case DKIMCheckSignatureJob::DKIMWarning::SignatureTooSmall:
        tooltip = i18n("Signature too small");
        break;
    }
    mLabel->setToolTip(tooltip);
}
