/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "remotecontentconfigurewidget.h"
#include <KLocalizedString>
#include <QListWidget>
#include <QVBoxLayout>
using namespace MessageViewer;
RemoteContentConfigureWidget::RemoteContentConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mListWidget = new QListWidget(this);
    mListWidget->setObjectName(QStringLiteral("mListWidget"));
    mainLayout->addWidget(mListWidget);
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget()
{

}

void RemoteContentConfigureWidget::saveSettings()
{

}

void RemoteContentConfigureWidget::readSettings()
{

}
