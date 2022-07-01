/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KJob>
#include <QDBusPendingReply>

#include "messagecomposer_export.h"

class OrgFreedesktopAkonadiSendLaterAgentInterface;

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT SendLaterJob : public KJob
{
    Q_OBJECT
public:
    enum Error {
        AgentNotAvailable = KJob::UserDefinedError,
        CallFailed,
    };

    explicit SendLaterJob(QObject *parent = nullptr);

    void start() final;

protected:
    virtual QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) = 0;
    virtual QString getErrorString(Error error, const QString &desc) const = 0;
};
}
