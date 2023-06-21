/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
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

#include "moc_sendlaterremovejob.cpp"
