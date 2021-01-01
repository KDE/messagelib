/*
  SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "viewerpurposemenuwidget.h"

#include <MessageViewer/MailWebEngineView>

using namespace MessageViewer;

MailfilterPurposeMenuWidget::MailfilterPurposeMenuWidget(QWidget *parentWidget, QObject *parent)
    : PimCommon::PurposeMenuWidget(parentWidget, parent)
{
}

MailfilterPurposeMenuWidget::~MailfilterPurposeMenuWidget()
{
}

QByteArray MailfilterPurposeMenuWidget::text()
{
    if (mViewer) {
        return mViewer->selectedText().toUtf8();
    }
    return {};
}

void MailfilterPurposeMenuWidget::setViewer(MailWebEngineView *viewer)
{
    mViewer = viewer;
}
