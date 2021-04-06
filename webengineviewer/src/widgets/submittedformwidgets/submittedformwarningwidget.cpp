/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "submittedformwarningwidget.h"

#include <KLocalizedString>

using namespace WebEngineViewer;

SubmittedFormWarningWidget::SubmittedFormWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("Submit form is not allowed in mailer. Please open url in a browser."));
}

SubmittedFormWarningWidget::~SubmittedFormWarningWidget()
{
}

void SubmittedFormWarningWidget::showWarning()
{
    animatedShow();
}
