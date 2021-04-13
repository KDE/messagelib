/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include "sendlaterjob.h"

#include <AkonadiCore/Item>

namespace MessageComposer
{
class MESSAGECOMPOSER_EXPORT SendLaterRemoveJob : public SendLaterJob
{
    Q_OBJECT
public:
    explicit SendLaterRemoveJob(Akonadi::Item::Id item, QObject *parent = nullptr);

protected:
    Q_REQUIRED_RESULT QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) override;
    Q_REQUIRED_RESULT QString getErrorString(Error error, const QString &detail) const override;

private:
    const Akonadi::Item::Id mItem;
};
}

