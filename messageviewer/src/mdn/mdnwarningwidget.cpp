/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mdnwarningwidget.h"
#include <KLocalizedString>

MDNWarningWidget::MDNWarningWidget(QWidget *parent)
    : KMessageWidget(parent)
{
    setVisible(false);
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    // TODO
    // setText(i18n("This message may be a scam. <a href=\"scamdetails\">(Details...)</a>"));
}

MDNWarningWidget::~MDNWarningWidget() = default;
