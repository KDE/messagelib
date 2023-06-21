/*
    This file is part of KDE.

    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwindowpositioner.h"

#include "messagecomposer_debug.h"
#include <QScreen>

using namespace MessageComposer;
KWindowPositioner::KWindowPositioner(QWidget *master, QWidget *slave, Mode mode)
    : QObject(master)
    , mMaster(master)
    , mSlave(slave)
    , mMode(mode)
{
}

void KWindowPositioner::reposition()
{
    QPoint relativePos;
    if (mMode == Right) {
        relativePos = QPoint(mMaster->width(), 0);
    } else if (mMode == Bottom) {
        relativePos = QPoint(mMaster->width() - mSlave->frameGeometry().width(), mMaster->height());
    } else {
        qCCritical(MESSAGECOMPOSER_LOG) << "KWindowPositioner: Illegal mode";
    }
    QPoint pos = mMaster->mapToGlobal(relativePos);

    // fix position to avoid hiding parts of the window (needed especially when not using KWin)
    const QRect desktopRect(mMaster->screen()->availableGeometry());
    if ((pos.x() + mSlave->frameGeometry().width()) > desktopRect.width()) {
        pos.setX(desktopRect.width() - mSlave->frameGeometry().width());
    }
    if ((pos.y() + mSlave->frameGeometry().height()) > desktopRect.height()) {
        pos.setY(desktopRect.height() - mSlave->frameGeometry().height() - mMaster->height());
    }
    qCDebug(MESSAGECOMPOSER_LOG) << mMaster->pos() << mMaster->mapToGlobal(mMaster->pos()) << pos.y() << (mMaster->pos().y() - pos.y())
                                 << mSlave->frameGeometry().height();
    if (mMode == Bottom && mMaster->mapToGlobal(mMaster->pos()).y() > pos.y() && (mMaster->pos().y() - pos.y()) < mSlave->frameGeometry().height()) {
        pos.setY(mMaster->mapToGlobal(QPoint(0, -mSlave->frameGeometry().height())).y());
    }
    if (pos.x() < desktopRect.left()) {
        pos.setX(desktopRect.left());
    }
    if (pos.y() < desktopRect.top()) {
        pos.setY(desktopRect.top());
    }

    mSlave->move(pos);
    mSlave->raise();
}

#include "moc_kwindowpositioner.cpp"
