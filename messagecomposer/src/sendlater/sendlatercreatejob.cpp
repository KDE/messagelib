/*
   Copyright (C) 2020 Daniel Vrátil <dvratil@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "sendlatercreatejob.h"
#include "sendlaterinterface.h"

#include <KLocalizedString>

using namespace MessageComposer;

SendLaterCreateJob::SendLaterCreateJob(const SendLaterInfo &info, QObject *parent)
    : SendLaterJob(parent)
    , mInfo(info)
{}

QDBusPendingReply<> SendLaterCreateJob::doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface)
{
    return iface->addItem(mInfo.dateTime().toSecsSinceEpoch(), mInfo.isRecurrence(), mInfo.recurrenceEachValue(), mInfo.recurrenceUnit(),
                          mInfo.itemId(), mInfo.subject(), mInfo.to());
}

QString SendLaterCreateJob::getErrorString(Error, const QString &detail) const
{
    return i18n("Failed to schedule message for sending later: %1", detail);
}
