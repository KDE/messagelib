/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "tablockedwarning.h"
#include <KLocalizedString>
using namespace MessageViewer;

TabLockedWarning::TabLockedWarning(QWidget *parent)
    : KMessageWidget(parent)
{
    setCloseButtonVisible(true);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("Submit form is not allowed in mailer. Please open url in a browser."));
}

TabLockedWarning::~TabLockedWarning()
{

}
