/*
  SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "messagewidgetarea.h"
#include <QVBoxLayout>
using namespace MessageViewer;
MessageWidgetArea::MessageWidgetArea(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});
}

MessageWidgetArea::~MessageWidgetArea() = default;

#include "moc_messagewidgetarea.cpp"
