/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "shownextmessagewidget.h"
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QPushButton>

using namespace MessageViewer;
ShowNextMessageWidget::ShowNextMessageWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins({});

    mPreviousMessage = new QPushButton(i18n("Previous Message"), this);
    mPreviousMessage->setObjectName(QStringLiteral("previous_message"));
    mPreviousMessage->setEnabled(false);
    mainLayout->addWidget(mPreviousMessage);
    connect(mPreviousMessage, &QPushButton::clicked, this, &ShowNextMessageWidget::showPreviousMessage);

    mainLayout->addStretch(1);
    mNextMessage = new QPushButton(i18n("Next Message"), this);
    mNextMessage->setObjectName(QStringLiteral("next_message"));
    mNextMessage->setEnabled(false);
    connect(mNextMessage, &QPushButton::clicked, this, &ShowNextMessageWidget::showNextMessage);
    mainLayout->addWidget(mNextMessage);
    setMaximumHeight(mNextMessage->height() + 4);
}

ShowNextMessageWidget::~ShowNextMessageWidget()
{
}

void ShowNextMessageWidget::updateButton(bool hasPreviousMessage, bool hasNextMessage)
{
    mPreviousMessage->setEnabled(hasPreviousMessage);
    mNextMessage->setEnabled(hasNextMessage);
}
