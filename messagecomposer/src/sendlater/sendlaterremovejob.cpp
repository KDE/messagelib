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

#include "sendlaterremovejob.h"
#include "sendlaterinterface.h"

#include <KLocalizedString>

using namespace MessageComposer;

SendLaterRemoveJob::SendLaterRemoveJob(Akonadi::Item::Id item, QObject *parent)
    : SendLaterJob(parent)
    , mItem(item)
{
}

QDBusPendingReply<> SendLaterRemoveJob::doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface)
{
    return iface->removeItem(mItem);
}

QString SendLaterRemoveJob::getErrorString(Error, const QString &detail) const
{
    return i18n("Failed to removed message from send later schedule: %1", detail);
}
