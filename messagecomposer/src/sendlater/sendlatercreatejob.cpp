/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlatercreatejob.h"
#include "sendlaterinterface.h"

#include <KLocalizedString>

using namespace MessageComposer;

SendLaterCreateJob::SendLaterCreateJob(const SendLaterInfo &info, QObject *parent)
    : SendLaterJob(parent)
    , mInfo(info)
{
}

QDBusPendingReply<> SendLaterCreateJob::doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface)
{
    return iface->addItem(mInfo.dateTime().toSecsSinceEpoch(),
                          mInfo.isRecurrence(),
                          mInfo.recurrenceEachValue(),
                          mInfo.recurrenceUnit(),
                          mInfo.itemId(),
                          mInfo.subject(),
                          mInfo.to());
}

QString SendLaterCreateJob::getErrorString(Error, const QString &detail) const
{
    return i18n("Failed to schedule message for sending later: %1", detail);
}

#include "moc_sendlatercreatejob.cpp"
