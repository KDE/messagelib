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
    setText(QStringLiteral("%1 <a href=\"unlock\">%2</a>").arg(i18n("Current Tab is locked."), i18n("(Unlock it)")));
    setVisible(false);
    connect(this, &TabLockedWarning::linkActivated, this, &TabLockedWarning::slotLinkActivated);
}

TabLockedWarning::~TabLockedWarning()
{
}

void TabLockedWarning::slotLinkActivated(const QString &contents)
{
    if (contents == QLatin1String("unlock")) {
        Q_EMIT unlockTabRequested();
    }
}
