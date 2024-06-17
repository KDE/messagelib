/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "shownextmessagewidget.h"
#include <KLocalizedString>
#include <QHBoxLayout>
#include <QPushButton>

using namespace MessageViewer;
ShowNextMessageWidget::ShowNextMessageWidget(QWidget *parent)
    : QWidget(parent)
    , mNextMessage(new QPushButton(i18nc("@action:button", "Next Message"), this))
    , mPreviousMessage(new QPushButton(i18nc("@action:button", "Previous Message"), this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));
    mainLayout->setContentsMargins({});

    mPreviousMessage->setObjectName(QLatin1StringView("previous_message"));
    mPreviousMessage->setEnabled(false);
    mainLayout->addWidget(mPreviousMessage);
    connect(mPreviousMessage, &QPushButton::clicked, this, &ShowNextMessageWidget::showPreviousMessage);

    mainLayout->addStretch(1);
    mNextMessage->setObjectName(QLatin1StringView("next_message"));
    mNextMessage->setEnabled(false);
    connect(mNextMessage, &QPushButton::clicked, this, &ShowNextMessageWidget::showNextMessage);
    mainLayout->addWidget(mNextMessage);
    setMaximumHeight(mNextMessage->height() + 4);
}

ShowNextMessageWidget::~ShowNextMessageWidget() = default;

void ShowNextMessageWidget::updateButton(bool hasPreviousMessage, bool hasNextMessage)
{
    mPreviousMessage->setEnabled(hasPreviousMessage);
    mNextMessage->setEnabled(hasNextMessage);
}

#include "moc_shownextmessagewidget.cpp"
