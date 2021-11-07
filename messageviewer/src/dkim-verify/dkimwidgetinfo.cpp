/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimwidgetinfo.h"
#include "dkimmanager.h"
#include "dkimutil.h"
#include "messageviewer_dkimcheckerdebug.h"
#include <KColorScheme>
#include <KLocalizedString>

#include "dkimmailstatus.h"
#include <QHBoxLayout>
#include <QLabel>

using namespace MessageViewer;
DKIMWidgetInfo::DKIMWidgetInfo(QWidget *parent)
    : QWidget(parent)
    , mLabel(new QLabel(this))
    , mDKIMMailStatus(new DKIMMailStatus(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mLabel->setAutoFillBackground(true);
    mLabel->setObjectName(QStringLiteral("label"));
    mainLayout->addWidget(mLabel);
    connect(mDKIMMailStatus, &DKIMMailStatus::textChanged, this, [this]() {
        mLabel->setText(mDKIMMailStatus->text());
    });
    connect(mDKIMMailStatus, &DKIMMailStatus::tooltipChanged, this, [this]() {
        mLabel->setToolTip(mDKIMMailStatus->tooltip());
    });
    connect(mDKIMMailStatus, &DKIMMailStatus::statusChanged, this, [this]() {
        onStatusChanged(mDKIMMailStatus->status());
    });
    initColors();
}

DKIMWidgetInfo::~DKIMWidgetInfo()
{
}

void DKIMWidgetInfo::initColors()
{
    const KColorScheme colorScheme{QPalette::Active};
    mWarningColor = colorScheme.background(KColorScheme::NeutralBackground).color();
    mErrorColor = colorScheme.background(KColorScheme::NegativeBackground).color();
    mOkColor = colorScheme.background(KColorScheme::PositiveBackground).color();
    mDefaultColor = palette().window().color();
}

MessageViewer::DKIMCheckSignatureJob::CheckSignatureResult DKIMWidgetInfo::result() const
{
    return mDKIMMailStatus->result();
}

Akonadi::Item::Id DKIMWidgetInfo::currentItemId() const
{
    return mDKIMMailStatus->currentItemId();
}

void DKIMWidgetInfo::setCurrentItemId(Akonadi::Item::Id currentItemId)
{
    mDKIMMailStatus->setCurrentItemId(currentItemId);
}

void DKIMWidgetInfo::setResult(const DKIMCheckSignatureJob::CheckSignatureResult &checkResult, Akonadi::Item::Id id)
{
    mDKIMMailStatus->setResult(checkResult, id);
}

void DKIMWidgetInfo::clear()
{
    mDKIMMailStatus->clear();
}

void DKIMWidgetInfo::onStatusChanged(MessageViewer::DKIMCheckSignatureJob::DKIMStatus status)
{
    QPalette pal = mLabel->palette();
    switch (status) {
    case DKIMCheckSignatureJob::DKIMStatus::Unknown:
        pal.setColor(backgroundRole(), mDefaultColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Valid:
        pal.setColor(backgroundRole(), (mDKIMMailStatus->result().warning != DKIMCheckSignatureJob::DKIMWarning::Any) ? mWarningColor : mOkColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::Invalid:
        pal.setColor(backgroundRole(), mErrorColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::EmailNotSigned:
        pal.setColor(backgroundRole(), mDefaultColor);
        mLabel->setPalette(pal);
        break;
    case DKIMCheckSignatureJob::DKIMStatus::NeedToBeSigned:
        pal.setColor(backgroundRole(), mErrorColor);
        mLabel->setPalette(pal);
        break;
    }
}
