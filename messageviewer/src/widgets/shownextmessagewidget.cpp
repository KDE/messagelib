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

#include "shownextmessagewidget.h"
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QPushButton>

using namespace MessageViewer;
ShowNextMessageWidget::ShowNextMessageWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins(0, 0, 0, 0);


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
