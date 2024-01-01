/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "statusbarlabeltoggledstate.h"
#include "messagecomposer_debug.h"
#include <QMouseEvent>
using namespace MessageComposer;
StatusBarLabelToggledState::StatusBarLabelToggledState(QWidget *parent)
    : QLabel(parent)
{
    setTextFormat(Qt::PlainText);
}

StatusBarLabelToggledState::~StatusBarLabelToggledState() = default;

void StatusBarLabelToggledState::setStateString(const QString &toggled, const QString &untoggled)
{
    if (toggled.isEmpty() && untoggled.isEmpty()) {
        qCWarning(MESSAGECOMPOSER_LOG) << " State string is empty. Need to fix it";
    }
    mToggled = toggled;
    mUnToggled = untoggled;
    updateLabel();
}

void StatusBarLabelToggledState::setToggleMode(bool state)
{
    if (mToggleMode != state) {
        mToggleMode = state;
        Q_EMIT toggleModeChanged(mToggleMode);
        updateLabel();
    }
}

bool StatusBarLabelToggledState::toggleMode() const
{
    return mToggleMode;
}

void StatusBarLabelToggledState::updateLabel()
{
    if (mToggleMode) {
        setText(mToggled);
    } else {
        setText(mUnToggled);
    }
}

void StatusBarLabelToggledState::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev)
    if (ev->button() == Qt::LeftButton) {
        setToggleMode(!mToggleMode);
    }
}

#include "moc_statusbarlabeltoggledstate.cpp"
