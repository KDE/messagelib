/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "tablockedwarning.h"
#include <KLocalizedString>
#include <QAction>
using namespace MessageList::Core;
TabLockedWarning::TabLockedWarning(QWidget *parent)
    : KMessageWidget(parent)
{
    setCloseButtonVisible(false);
    setMessageType(Warning);
    setWordWrap(true);
    setText(i18n("Current Tab is locked."));
    setVisible(false);
    auto unlockAction = new QAction(i18n("Unlock"), this);
    connect(unlockAction, &QAction::triggered, this, &TabLockedWarning::unlockTabRequested);
    addAction(unlockAction);
}

TabLockedWarning::~TabLockedWarning()
{
}
