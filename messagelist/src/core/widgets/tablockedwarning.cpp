/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "tablockedwarning.h"
using namespace Qt::Literals::StringLiterals;

#include <KLocalizedString>
using namespace MessageList::Core;
TabLockedWarning::TabLockedWarning(QWidget *parent)
    : KMessageWidget(parent)
{
    setCloseButtonVisible(false);
    setMessageType(Warning);
    setWordWrap(true);
    setText(u"%1 <a href=\"unlock\">%2</a>"_s.arg(i18n("Current Tab is locked."), i18n("(Unlock it)")));
    setVisible(false);
    setPosition(KMessageWidget::Header);
    connect(this, &TabLockedWarning::linkActivated, this, &TabLockedWarning::slotLinkActivated);
}

TabLockedWarning::~TabLockedWarning() = default;

void TabLockedWarning::slotLinkActivated(const QString &contents)
{
    if (contents == QLatin1StringView("unlock")) {
        Q_EMIT unlockTabRequested();
    }
}

#include "moc_tablockedwarning.cpp"
