/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>

  Based on kmail/kmlineeditspell.h/cpp
  SPDX-FileCopyrightText: 1997 Markus Wuebben <markus.wuebben@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "composerlineedit.h"
#include <KCompletionBox>
#include <QKeyEvent>

#include "settings/messagecomposersettings.h"

using namespace MessageComposer;

ComposerLineEdit::ComposerLineEdit(bool useCompletion, QWidget *parent)
    : PimCommon::AddresseeLineEdit(parent, useCompletion)
{
    allowSemicolonAsSeparator(MessageComposerSettings::allowSemicolonAsAddressSeparator());
    setShowRecentAddresses(MessageComposerSettings::self()->showRecentAddressesInComposer());
    setRecentAddressConfig(MessageComposerSettings::self()->config());
    loadContacts();
    setEnableBalooSearch(MessageComposerSettings::showBalooSearchInComposer());
}

ComposerLineEdit::~ComposerLineEdit() = default;

//-----------------------------------------------------------------------------
void ComposerLineEdit::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) && !completionBox()->isVisible()) {
        Q_EMIT focusDown();
        AddresseeLineEdit::keyPressEvent(e);
        return;
    } else if (e->key() == Qt::Key_Up) {
        Q_EMIT focusUp();
        return;
    } else if (e->key() == Qt::Key_Down) {
        Q_EMIT focusDown();
        return;
    }
    AddresseeLineEdit::keyPressEvent(e);
}

#include "moc_composerlineedit.cpp"
