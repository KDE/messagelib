/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "remotecontentconfigurewidget.h"
#include <KLocalizedString>
#include <QVBoxLayout>
using namespace MessageViewer;
RemoteContentConfigureWidget::RemoteContentConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
    auto vboxLayout = new QVBoxLayout(this);
    vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
    vboxLayout->setContentsMargins({});
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget()
{

}
