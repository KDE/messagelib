/*
  SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "viewerpurposemenuwidget.h"

#include <MessageViewer/MailWebEngineView>

using namespace MessageViewer;

ViewerPurposeMenuWidget::ViewerPurposeMenuWidget(QWidget *parentWidget, QObject *parent)
    : PimCommon::PurposeMenuWidget(parentWidget, parent)
{
}

ViewerPurposeMenuWidget::~ViewerPurposeMenuWidget() = default;

QByteArray ViewerPurposeMenuWidget::text()
{
    if (mViewer) {
        return mViewer->selectedText().toUtf8();
    }
    return {};
}

void ViewerPurposeMenuWidget::setViewer(MailWebEngineView *viewer)
{
    mViewer = viewer;
}

#include "moc_viewerpurposemenuwidget.cpp"
