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
/*!
 * \class MessageComposer::SendLaterCreateJob
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/SendLaterCreateJob
 * \brief Create a send later job
 */
class MESSAGECOMPOSER_EXPORT SendLaterCreateJob : public SendLaterJob
{
    Q_OBJECT
public:
    /*! \brief Constructs a SendLaterCreateJob.
        \param info The send later information containing scheduling details.
        \param parent The parent object.
    */
    explicit SendLaterCreateJob(const SendLaterInfo &info, QObject *parent = nullptr);

protected:
    [[nodiscard]] QDBusPendingReply<> doCall(OrgFreedesktopAkonadiSendLaterAgentInterface *iface) override;
    [[nodiscard]] QString getErrorString(SendLaterJob::Error code, const QString &detail) const override;

private:
    const SendLaterInfo mInfo;
};
}
