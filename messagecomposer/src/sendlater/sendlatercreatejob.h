/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "sendlaterinfo.h"
#include "sendlaterjob.h"

#include "messagecomposer_export.h"

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT SendLaterCreateJob : public SendLaterJob
{
    Q_OBJECT
public:
    explicit SendLaterCreateJob(const SendLaterInfo &info, QObject *parent = nullptr);

protected:
    [[nodiscard]] QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) override;
    [[nodiscard]] QString getErrorString(SendLaterJob::Error code, const QString &detail) const override;

private:
    const SendLaterInfo mInfo;
};
}
